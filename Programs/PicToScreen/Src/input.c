/**
 ******************************************************************************
 * @file    input.c
 * @brief   Protokoll-/Dateischicht des Input-Interfaces (Burst-Verwaltung +
 *          Handshake), Implementierung von input.h.
 *
 *          Ziel: NUCLEO-F429ZI, OTG_FS, Connector CN13.
 *
 *          SCHICHTUNG: Diese Datei enthaelt nur die Protokoll-/Dateischicht:
 *            - Handshake ('H'/'R'/'G', 'F' bzw. 'S' vor JEDER Datei)
 *            - Burst-Verwaltung ([len:2][len Bytes] ... [len = 0])
 *            - dateiartige API: open_nxt_file, nxt_chr, read_file, eof
 *
 *          Alles Transport-Abhaengige (Flow-Control, Ringpuffer, Endpoint-Arm,
 *          CDC-Tx bzw. USART3-DMA) liegt in der Transportschicht (byte_io.c,
 *          byte_io_usb.c, byte_io_uart.c); diese Datei sieht davon nur die
 *          blockierenden Byte-Primitive aus byte_io.h.
 *
 *          INIT-KASKADE: init_input() ruft als ersten Schritt byte_io_init()
 *          auf -- so ist die Reihenfolge "erst Transport, dann Handshake auf
 *          dem fertigen Kanal" konstruktiv gesichert und kann vom Aufrufer
 *          nicht falsch gemacht werden.
 *
 *          PROTOKOLL (Transport wird in init_input gewaehlt; die PC-Anwendung
 *          erkennt ihn am Start-Kommando):
 *            PC ---- 'H' --->  <--- 'R' ---  PC ---- 'G' --->
 *            USB-CDC:        STM32 --- 'F' --->  (vor JEDER Datei)
 *                            PC: [len:2][len Bytes] ... [len = 0]  (blind, ohne 'D';
 *                                Flusskontrolle per USB-Endpoint-NAK)
 *            UART-via-USB:   STM32 --- 'S' --->  (vor JEDER Datei), dann pro Burst
 *                            STM32 --- 'D' --->  PC: [len Bytes]  (Request/Response;
 *                                UART hat keine HW-Flusskontrolle)
 *                            ABBRUCH: 'S' mitten in einer Datei (statt 'D') bricht
 *                                die Datei auf PC-Seite ab und startet die
 *                                naechste -- Restbytes + der eine in-flight Burst
 *                                werden verworfen (abort_file_uart()).
 *
 *          EOF-SEMANTIK: eof() ist die einzige Stelle, die den Dateiende-
 *          Zustand feststellt (no_elems_in_buf == 0 nach Prefetch des
 *          naechsten Laengenheaders). nxt_chr() nutzt ausschliesslich eof().
 *          Wer EOF ohne Konsumieren eines Bytes abfragen will
 *          (Schleifen-Steuerung), ruft eof() direkt.
 ******************************************************************************
 */

#include "input.h"
#include "byte_io.h"
#include "errorhandler.h"
#include "lcd.h"
#include "timer.h"
#include "transport.h" /* transport_t, TRANSPORT_USB_CDC, TRANSPORT_UART_VIA_USB */
#include "stm32f429xx.h" /* GPIOF (CMSIS Device Header, Leaf-Header des F429) */
#include "stm32f4xx_hal_gpio.h" /* HAL_GPIO_ReadPin, GPIO_PIN_5/6/7, GPIO_PIN_RESET */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> /* EOF */

/*
 * Commands and constants for transfer protocol
 */
#define HELLO_IN_CMD 'H'             // Hello cmd send from Python program until our
                                     // programs answers with READY
#define READY_OUT_CMD 'R'            // Ready cmd send to Python program
#define GO_IN_CMD 'G'                // Send by Python program after READY; Start.
#define START_OUT_CMD_WITHOUT_FC 'F' // USB-CDC: wird vor JEDER Datei gesendet; danach
                                     // streamt der PC die komplette Datei blind
                                     // (Bursts, len=0 am Ende; Flusskontrolle per USB-NAK)
#define START_OUT_CMD 'S'            // UART-via-USB: vor JEDER Datei; der PC antwortet
                                     // pro Burst auf 'D' (UART hat keine HW-Flusskontrolle)
#define DATA_OUT_CMD 'D'             // UART-via-USB: naechsten Burst anfordern

#define BUF_SIZE 512 // Number of bytes of one burst (Protokollkonstante)

/*
 * LCD-Zeilen fuer Transport-Auswahl und S5-Hinweis (Default-Font 16 px:
 * lcdGotoXY(1,y) belegt Pixel y*16..y*16+15).
 */
enum MenuRow {
   MENU_ROW_TITLE = 1,     /* "Uebertragung waehlen:" */
   MENU_ROW_S7 = 3,        /* S7 (PF7) = USB-CDC */
   MENU_ROW_S6 = 4,        /* S6 (PF6) = UART-via-USB */
   MENU_ROW_WAIT = 6,      /* "Bitte Taster druecken ..." */
   MENU_ROW_SELECTED = 8,  /* "Gewaehlt: ..." */
   MENU_ROW_S5_HINT = 10   /* S5-Hinweistext */
};

/* ========================================================================== */
/* Burst-Verwaltung (KEIN Prefetch -- USB-Flow-Control regelt das Tempo)      */
/* ========================================================================== */

static int next_char_pos = BUF_SIZE; // next unread element in buffer (BUF_SIZE :
                                     // alle elems has been consumed)
static int no_elems_in_buf = 0;      // number of elems in buffer; no_elems_in_buf == 0 : EOF

uint32_t ts_prev, ts_now;
uint32_t diff[ 1024]; // NOLINT(readability-magic-numbers)
int32_t diff_pos = 0;

/**
 * @brief  Liest den 2-Byte-Laengenheader des naechsten Bursts.
 * @param  None
 * @retval None (Ergebnis in no_elems_in_buf / next_char_pos)
 */
static void read_burst_header(void) {
   // read no of data that will be transmitted
   no_elems_in_buf = (byte_io_get() & 0xFF) << 8;              // NOLINT(readability-magic-numbers)
   no_elems_in_buf = no_elems_in_buf | (byte_io_get() & 0xFF); // NOLINT(readability-magic-numbers)
   /*
    * Protection: Burst-Laenge darf BUF_SIZE nicht ueberschreiten.
    */
   CHECK_FATAL(no_elems_in_buf <= BUF_SIZE, INTERNAL_ERR); // Burst size to large
   next_char_pos = 0;
}

/**
 * @brief  Holt den naechsten Burst: liest den 2-Byte-Laengenheader und fordert
 *         bei UART-via-USB mit len != 0 per 'D' die Burst-Daten an (ein nacktes
 *         read_burst_header() wuerde den PC nicht zum Senden veranlassen -- der
 *         PC wartet auf 'D'; der Drain haengt dann ab dem zweiten Burst).
 * @param  None
 * @retval None (Ergebnis in no_elems_in_buf / next_char_pos)
 */
static void fetch_burst(void) {
   bool uart = (byte_io_transport() == TRANSPORT_UART_VIA_USB);
   read_burst_header();
   /* Nur UART-via-USB: naechsten Burst explizit anfordern ('D') -- UART
      kann nicht NAKen wie der USB-Host. Im USB-Fall regelt die Hardware
      die Flusskontrolle, der PC streamt blind nach 'F'. */
   if (uart && (0 != no_elems_in_buf)) {
      byte_io_put(DATA_OUT_CMD);
   }
}

/**
 * @brief  Konsumiert die Restbytes der aktuellen Datei bis zur EOF-Marke
 *         (len == 0). Pro Burst wird fetch_burst() aufgerufen -- bei
 *         UART-via-USB inklusive 'D'-Request.
 * @param  None
 * @retval None
 */
static void drain_to_eof(void) {
   while (no_elems_in_buf != 0) {
      while (next_char_pos < no_elems_in_buf) {
         (void)byte_io_get();
         next_char_pos++;
      }
      fetch_burst(); /* naechster Burst; bei UART inkl. 'D' */
   }
}

/**
 * @brief  Bricht die aktuelle Datei bei UART-via-USB ab, statt sie bis zur
 *         EOF-Marke zu drainen. Der Drain kostet pro Burst ein 'D'-
 *         Roundtrip -- bei grossen Dateien zig Sekunden, bis der naechste
 *         Header erscheint (Starthilfe liest pro Datei nur den Header).
 *
 *         Protokoll: Der PC sendet nach jedem empfangenen Kommando ('S'/'D')
 *         genau EINEN Burst und wertet jedes Kommando != 'D' als Abbruch der
 *         aktuellen Datei (gui.py: send_file_with_flow_control -> push_back +
 *         break; send_files startet mit dem zurueckgelegten 'S' die naechste
 *         Datei). Das 'D' wird immer direkt nach dem Lesen eines Laengen-
 *         headers mit len != 0 gesendet, daher liegt beim Abbruch genau ein
 *         weiterer Burst ([len:2][Daten]) auf der Leitung. Diese Funktion
 *         konsumiert den Rest des aktuellen Bursts und diesen einen in-flight
 *         Burst vollstaendig (KEIN weiteres 'D'); der Aufrufer leert danach
 *         den Ring und sendet 'S'. Der naechste read_burst_header() liest dann
 *         den Laengenheader der NEUEN Datei. Der Fall "in-flight Burst ist die
 *         EOF-Marke (len == 0)" ist durch read_burst_header() abgedeckt.
 * @param  None
 * @retval None
 */
static void abort_file_uart(void) {
   while (next_char_pos < no_elems_in_buf) { /* Rest des aktuellen Bursts */
      (void)byte_io_get();
      next_char_pos++;
   }
   read_burst_header(); /* Laengenheader des in-flight Bursts (alte Datei) */
   while (next_char_pos < no_elems_in_buf) { /* Daten des in-flight Bursts */
      (void)byte_io_get();
      next_char_pos++;
   }
}

/**
 * @brief  Startet den Empfang des naechsten Bursts bzw. einer neuen Datei.
 * @param open_new_file  A new file should be opened
 * @retval None
 */
static void start_next_byte_burst(bool open_new_file) {
   bool uart = (byte_io_transport() == TRANSPORT_UART_VIA_USB);
   if (open_new_file) {
      /* Serieller Neustart: Paketreste der bisherigen Datei duerfen nicht als
         neue Datei ankommen, dann lokalen Ring leeren, dann Start-Kommando
         an den PC. 'F' = USB-CDC (blind stream, USB-Flow-Control),
         'S' = UART-via-USB (Burst-Request per 'D').
         UART-via-USB mit noch ungelesenen Restbytes: Datei ABBRECHEN statt
         drainen -- der PC wertet das 'S' als Abbruch der aktuellen Datei und
         startet die naechste (statt eines 'D'-Roundtrips pro Rest-Burst). */
      if (uart && (0 != no_elems_in_buf)) {
         abort_file_uart(); /* Restbytes + in-flight Burst verwerfen, kein 'D' */
      } else {
         drain_to_eof(); /* USB-CDC (blind stream) oder bereits EOF */
      }
      byte_io_clear();
      byte_io_put(uart ? START_OUT_CMD : START_OUT_CMD_WITHOUT_FC);
   }
   fetch_burst();
}

/**
 * @brief  Holt den naechsten Burst (blockierend), sobald der aktuelle
 *         aufgebraucht ist -- inkl. Zeitmessung der Burstabstaende.
 * @param  None
 * @retval None
 */
static void refill_burst_if_exhausted(void) {
   if ((no_elems_in_buf == next_char_pos) || (BUF_SIZE == next_char_pos)) {
      ts_prev = ts_now;
      ts_now = getTimeStamp();
      diff[diff_pos] = ((ts_now - ts_prev) / (uint32_t)(168 / 2)) / (uint32_t) 1000; // NOLINT(readability-magic-numbers)
      diff_pos = (diff_pos + 1) % (int) (sizeof(diff) / sizeof(diff[0]));

      start_next_byte_burst(false);
   }
}

/* ========================================================================== */
/* Oeffentliches Interface (input.h)                                          */
/* ========================================================================== */

/**
 * @brief  Fragt auf dem LCD ab, welcher Transport fuer die Uebertragung
 *         genutzt werden soll, und blockiert, bis ein Taster gedrueckt wird:
 *         S7 (PF7) = USB-CDC, S6 (PF6) = UART-via-USB.
 *         Taster sind active-low (Pull-up aus initITSboard()/initGPIOs).
 *         Textausgabe wie im Error-Handler ueber lcdGotoXY()/lcdPrintS().
 *         Der S5-Hilfetext erscheint nicht hier, sondern einmalig nach
 *         Aufbau der Verbindung (show_s5_hint()) -- bei UART-via-USB kann
 *         der PC den Handshake schon vorher geliefert haben, dann wuerde
 *         das erste Bild den Text sofort wieder loeschen.
 * @param  None
 * @retval TRANSPORT_USB_CDC (S7) oder TRANSPORT_UART_VIA_USB (S6)
 */
transport_t ask_transport(void) {
   lcdGotoXY(1, MENU_ROW_TITLE);
   lcdPrintS(" Uebertragung waehlen:");
   lcdGotoXY(1, MENU_ROW_S7);
   lcdPrintS(" S7 (PF7) = USB-CDC");
   lcdGotoXY(1, MENU_ROW_S6);
   lcdPrintS(" S6 (PF6) = UART-via-USB");
   lcdGotoXY(1, MENU_ROW_WAIT);
   lcdPrintS(" Bitte Taster druecken ...");

   transport_t transport;
   for (;;) {
      if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7) == GPIO_PIN_RESET) { /* S7 gedrueckt */
         transport = TRANSPORT_USB_CDC;
         break;
      }
      if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_RESET) { /* S6 gedrueckt */
         transport = TRANSPORT_UART_VIA_USB;
         break;
      }
   }
   lcdGotoXY(1, MENU_ROW_SELECTED);
   lcdPrintS((TRANSPORT_USB_CDC == transport) ? " Gewaehlt: USB-CDC      " : " Gewaehlt: UART-via-USB ");
   return transport;
}

/**
 * @brief  Zeigt einmalig den Hilfetext fuer S5 auf dem LCD an
 *         (" S5 (PF5) = weiter / naechstes Bild"). Aufruf nach init_input()
 *         (Verbindung steht), direkt vor dem ersten wait_for_s_i(5) -- so ist
 *         sichergestellt, dass der Anwender den Text sieht, bevor das erste
 *         Bild das Display loescht. Danach wird ohne Textausgabe gewartet.
 * @param  None
 * @retval None
 */
void show_s5_hint(void) {
   lcdGotoXY(1, MENU_ROW_S5_HINT);
   lcdPrintS(" S5 (PF5) = weiter / naechstes Bild");
}

/**
 * @brief  Blockiert ohne Textausgabe, bis die Taste S_i vollstaendig
 *         gedrueckt wird (btn = BUTTON_S5..BUTTON_S7, d.h. S5/S6/S7 auf
 *         PF5/PF6/PF7; active-low wie in ask_transport(), Pull-up aus
 *         initITSboard()/initGPIOs). Dazu wird erst auf LOSLASSEN und dann
 *         auf DRUECKEN gewartet: Ein Tastendruck zaehlt genau einmal -- auch
 *         wenn er laenger gehalten wird und zwei wait_for_s_i()-Aufrufe
 *         direkt hintereinander liegen (sonst wuerde ein langer Druck beide
 *         Waits auf einmal erfuellen, ein kurzer nur den ersten).
 *
 * @param  btn  Taster-Nummer: BUTTON_S5/S6/S7 = S5 (PF5) / S6 (PF6) / S7 (PF7)
 * @retval None
 */
void wait_for_s_i(int btn) {
   CHECK_FATAL((BUTTON_S5 <= btn) && (btn <= BUTTON_S7), INDEX_ERR); /* nur S5..S7 auf PF5..PF7 */
   uint16_t pin = (uint16_t)(GPIO_PIN_5 << (btn - BUTTON_S5));
   while (HAL_GPIO_ReadPin(GPIOF, pin) == GPIO_PIN_RESET) { } /* erst loslassen ... */
   while (HAL_GPIO_ReadPin(GPIOF, pin) != GPIO_PIN_RESET) { } /* ... dann druecken */
}

/**
 * @brief This function setups communication protocol to Python send file
 * program
 * @param transport  TRANSPORT_USB_CDC (Default) oder TRANSPORT_UART_VIA_USB
 */
void init_input(transport_t transport) {
   /* 1. Transportschicht initialisieren (Dispatcher byte_io.c reicht an
         das gewaehlte Backend durch). Kaskade: erst Transport, dann
         Handshake auf dem fertigen Kanal. */
   byte_io_init(transport);

   /* 2. Protokoll-Handshake */
   char chr;
   while (HELLO_IN_CMD != (char)byte_io_get()) { }
   byte_io_put(READY_OUT_CMD);
   do {
      chr = (char)byte_io_get();
      if ((HELLO_IN_CMD != chr) && (GO_IN_CMD != chr)) {
         lcdPrintS("Unexpected input cmd received.");
      }
   } while (GO_IN_CMD != chr);

   next_char_pos = BUF_SIZE; // burst management: no data available yet
   no_elems_in_buf = 0;
}

/**
 * @brief This function asks the Python program to open a new file.
 *        If all files has been read, it blocks until a new file has been
 *        selected in Python program.
 *
 *  Restbytes der bisherigen Datei duerfen nicht als neue Datei ankommen:
 *  bei USB-CDC werden sie bis EOF konsumiert (Drain in start_next_byte_burst),
 *  bei UART-via-USB wird die Datei per 'S' abgebrochen (abort_file_uart) --
 *  erst dann wird 'F' bzw. 'S' gesendet.
 */
void open_nxt_file(void) {
   start_next_byte_burst(true);
}

/**
 * @brief  Fragt ab, ob die aktuelle Datei vollstaendig gelesen ist --
 *         OHNE dabei ein Datenbyte zu konsumieren.
 *
 *  Prefetch-Semantik: Ist der aktuelle Burst aufgebraucht, wird der
 *  naechste Laengenheader hier SELBST geholt (blockierend). Erst danach
 *  steht der EOF-Zustand fest -- ein Probe-Lesen per nxt_chr() ist nicht
 *  noetig. Ein true bleibt true, bis open_nxt_file() die naechste Datei
 *  startet.
 *
 *  EINZIGE Stelle, die den Dateiende-Zustand feststellt; nxt_chr() nutzt
 *  ausschliesslich diese Funktion.
 *
 * @param  None
 * @retval true: Dateiende erreicht (EOF-Marke len == 0 gelesen bzw.
 *         noch keine Datei geoeffnet)
 */
bool eof(void) {
   if (0 == no_elems_in_buf) {
      return true; /* EOF-Marke bereits gelesen */
   }
   refill_burst_if_exhausted();
   return 0 == no_elems_in_buf;
}

/**
 * @brief  This function reads the next byte from the current file.
 * @param  None
 * @retval Next byte of the current file or EOF (32 bit value for -1)
 */
int nxt_chr(void) {
   if (eof()) {
      return EOF;
   }
   int chr = byte_io_get();
   next_char_pos++;
   return chr;
}

/**
 * @brief  This function reads count elements of data, each size bytes long,
 *         from the current file.
 * @param  buf   Storage buffer for data
 * @param  size  Size of each data element
 * @param count Number of data elements
 * @retval Number of items read. If not enough data are available, EOF will be
 * returned.
 */
int read_file(char *buf, unsigned int size, unsigned int count) {
   for (unsigned int i = 0; i < size * count; i++) {
      int chr = nxt_chr();
      if (EOF == chr) {
         return EOF;
      }
      buf[i] = (char)chr;
   }
   return (int)count;
}
// EOF
