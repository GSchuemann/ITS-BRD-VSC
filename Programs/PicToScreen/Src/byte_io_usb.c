/**
 ******************************************************************************
 * @file    byte_io_usb.c
 * @brief   USB-CDC Backend der Transportschicht (Input-Pfad).
 *
 *          Ziel: NUCLEO-F429ZI, OTG_FS, Connector CN13.
 *
 *          Diese Datei enthaelt alles USB-Abhaengige des Empfangspfads:
 *            - USB-Flow-Control: einzige Arm-Stelle usb_rx_arm_when_space()
 *            - Empfangs-Hook cdc_collect() (Interrupt-Kontext, Ringpuffer-Write)
 *            - blockierende Byte-Primitive byte_io_usb_get/put/clear
 *              (fuer den Dispatcher byte_io.c)
 *            - Transport-Initialisierung byte_io_usb_init()
 *
 *          Darueber liegt die Protokoll-/Dateischicht (input.c): Burst-
 *          Verwaltung, Handshake, nxt_chr/read_file/eof. Sie inkludiert nur
 *          byte_io.h -- keine USB-Header, keine Ringpuffer-Interna.
 *
 *          PROTOKOLL (Sicht der Transportschicht -- Regelwerk liegt in input.c):
 *            PC ---- 'H' --->  <--- 'R' ---  PC ---- 'G' --->
 *            PC <--- 'F' ---   (vor JEDER Datei)
 *            PC: [len:2][len Bytes] ... [len = 0]   (blind, ohne 'D')
 *
 *          FLUSSKONTROLLE (aktuelles Design):
 *            Der PC sendet blind; das OUT-Endpoint wird nur wieder scharf
 *            geschaltet, wenn der Ring Platz fuer ein volles CDC-Paket
 *            (64 Bytes) hat. Solange NAKt die Hardware; der Host wiederholt
 *            automatisch. Massgeblich ist allein ringbuf_free() -- dieselbe
 *            Rechnung, mit der ringbuf_write ein Paket ablehnen wuerde, also
 *            kann die Arm-Entscheidung nicht von der Schreib-Ablehnung
 *            abweichen. Nach jedem konsumierten Byte wird ggf. sofort
 *            nachgeruestet, sobald wieder Platz ist (nicht erst bei leerem
 *            Ring). Das Ergebnis von USBD_CDC_ReceivePacket() wird geprueft;
 *            bei HAL-Ablehnung (BUSY) wird rx_armed zurueckgesetzt und spaeter
 *            erneut versucht (kein Zombie-Zustand "Flag gesetzt, Endpoint
 *            nicht aufgeruestet"). Ein Teil-Write (Paket in vollen Ring) ist
 *            ein harter Fehler (Error_Handler); die Diagnosezaehler
 *            rx_overflow* protokollieren Anzahl, Freiplatz laut Ring und
 *            verworfene Bytes.
 *
 *          ZUSAMMENSPIEL usbd_cdc_if.c:
 *              cdc_collect(Buf, *Len);
 *              usb_rx_arm_when_space();     // einzige Arm-Stelle
 ******************************************************************************
 */

#include "byte_io_backend.h"
#include "cdc_input_if.h"
#include "error.h" /* Error_Handler bei rx_overflow */
#include "ringbuf.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "usbd_def.h" /* USBD_OK */
#include "core_cm4.h" /* __disable_irq, __enable_irq, __get/set_PRIMASK */
#include <stdbool.h>
#include <stdint.h>

#define CDC_MAX_PKT 64 /* max. Bytes pro CDC-Paket (Full-Speed) */

/** Software-Spiegel des OUT-Endpoint-Zustands: 1 = aufgeruestet
 *  (Empfangsauftrag erteilt), 0 = "zu" (Der OTG-Core empfängt noch
 *   ein &4 Byte Packet und verzögert dann das ACK, bis die Daten
 *   abgeholt wurden).
 *
 *  "Aufgeruestet": HAL schreibt DOEPTSIZ (PKTCNT=1, XFRSIZ=64) und
 *  DOEPCTL CNAK -- der Core hat GENAU EINEN Empfangsauftrag. Nach dem
 *  empfangenen Paket ist dieser verbraucht und der Core NAKt (NAKSTS)
 *  jedes weitere OUT-Token. "Zu" ist KEIN Fehler, sondern der Warte-
 *  zustand, in dem der Host selbststaendig wiederholt: Kein Re-Arm bei
 *  vollem Ring -> NAK an den Host -> das IST die USB-Flow-Control.
 *
 *  1 wird NUR in usb_rx_arm_when_space() gesetzt (nur bei
 *  ringbuf_free() >= CDC_MAX_PKT, unter IRQ-Sperre); 0 in cdc_collect()
 *  (Paket konsumiert) bzw. bei HAL-Ablehnung (RxState belegt). Das Flag
 *  verhindert Doppel-Aufruesten durch Main-Loop und ISR (Race).
 *  volatile: Zugriff aus ISR- und Main-Kontext. */
static volatile bool rx_armed = false;

/** Diagnose: Anzahl Paket-Teilwrites (Ring war voll). Erwartung: 0. */
static volatile uint32_t rx_overflow = 0;

/** Diagnose: Freiplatz im Ring (ringbuf_free!) beim letzten Overflow. */
static volatile uint32_t rx_overflow_free_last = 0;

/** Diagnose: Bytes, die beim letzten Overflow verworfen wurden. */
static volatile uint32_t rx_overflow_lost_last = 0;

uint32_t input_get_rx_overflow(void) { return rx_overflow; }
uint32_t input_get_rx_overflow_free(void) { return rx_overflow_free_last; }
uint32_t input_get_rx_overflow_lost(void) { return rx_overflow_lost_last; }

/* ========================================================================== */
/* USB-Flow-Control: EINZIGE Arm-Stelle, atomar gegen USB-ISR                  */
/* ========================================================================== */

/**
 * @brief  Oeffnet das USB-OUT-Endpoint wieder (arm), sobald der Ringpuffer
 *         Platz fuer ein volles CDC-Paket hat.
 *
 *  Hintergrund: Das OUT-Endpoint ist nach dem Empfang eines 64-Byte-
 *  Pakets GESCHLOSSEN (die Hardware hat ihren Empfangsauftrag abgearbeitet
 *  und NAKt alles Weitere). Erst wenn das Paket von cdc_collect() in den
 *  Ringpuffer verschoben wurde, kann -- und muss -- das Endpoint hier
 *  explizit wieder geoeffnet werden. Das geschieht nur, wenn der Ring
 *  mindestens CDC_MAX_PKT (64) Bytes freien Platz hat; andernfalls bleibt
 *  das Endpoint zu, die Hardware NAKt und der Host wiederholt automatisch.
 *  Genau das ist die USB-Flow-Control des Projekts.
 *
 *  EINZIGE Funktion im Projekt, die diese Entscheidung trifft. Den
 *  Hardware-Aufruf USBD_CDC_ReceivePacket() erledigt sie ueber den
 *  Wrapper cdc_rx_arm() (usbd_cdc_if.c) -- USB-Interna bleiben dort.
 *
 *  Die Platz-Abfrage erfolgt UEBER DEN RING SELBST (ringbuf_free) --
 *  dieselbe Rechnung, mit der ringbuf_write ein Paket ablehnen wuerde.
 *  Oeffnen-Entscheidung und Schreib-Ablehnung koennen damit prinzipiell
 *  nicht auseinanderlaufen.
 *
 *  Konsistenz von head/tail waehrend der Abfrage:
 *   - aus dem Main: __disable_irq() friert den USB-ISR ein
 *   - aus dem ISR:  Main kann nicht gleichzeitig laufen
 *
 *  @param  None
 * @retval None
 */
void usb_rx_arm_when_space(void) {
   uint32_t primask = __get_PRIMASK();  // NOLINT(misc-include-cleaner)
   __disable_irq();  // NOLINT(misc-include-cleaner) /* atomar gegen USB-ISR: kein Doppel-Aufruesten */
   if (!rx_armed && (ringbuf_free() >= CDC_MAX_PKT)) {
      rx_armed = true;
      if (!cdc_rx_arm()) {
         /* HAL hat abgelehnt (RxState belegt) -- Zombiezustand verhindern:
            Flag zuruecksetzen, naechster Versuch kommt aus dem Main/ISR. */
         rx_armed = false;
      }
   }
   __set_PRIMASK(primask);  // NOLINT(misc-include-cleaner)
}

/* ========================================================================== */
/* CDC-Hook: wird aus cdc_receive_fs() (usbd_cdc_if.c) aufgerufen            */
/* ========================================================================== */

/**
 * @brief  Legt ein empfangenes CDC-Paket im Ringpuffer ab.
 *
 *  Race-freie Reihenfolge:
 *    1. Bytes in den Ring schreiben (nur die geschriebene Anzahl zaehlt).
 *    2. Bei Teil-Write: Diagnosewerte sichern (Freiplatz laut RING!).
 *    3. rx_armed loeschen -- INNERHALB einer IRQ-Sperre. Erst danach kann
 *       der Main-Loop das Endpoint wieder auf ruesten; ein Zwischen-
 *       zustand ist von aussen nie sichtbar.
 *
 *  Das erneute Oeffnen des Endpoints macht ausschliesslich
 *  usb_rx_arm_when_space().
 *
 * @param  data  Zeiger auf die empfangenen Bytes.
 * @param  len   Anzahl der Bytes (max. 64 je CDC-Paket).
 * @retval None
 */
void cdc_collect(uint8_t *data, uint32_t len) {
   uint32_t written = ringbuf_write(data, len);
   if (written != len) {
      /* Freiplatz NACH dem Teil-Write; <64 beweist: Paket kam in fast
         vollen Ring -> Ursache im USB-Pfad, nicht im Ring. */
      rx_overflow_free_last = ringbuf_free();
      rx_overflow_lost_last = len - written;
      rx_overflow++;
      /* Datenverlust (Teilwrite) ist ein harter Fehler */
      Error_Handler();
   }

   uint32_t primask = __get_PRIMASK();  // NOLINT(misc-include-cleaner)
   __disable_irq();  // NOLINT(misc-include-cleaner)
   rx_armed = false; /* Endpoint verbraucht; Arm-Entscheidung folgt atomar */
   __set_PRIMASK(primask);  // NOLINT(misc-include-cleaner)
}

/* ========================================================================== */
/* Byte-Primitive fuer die Protokollschicht (input.c, byte_io.h)             */
/* ========================================================================== */

/**
 * @brief  Liest das naechste Byte aus dem Empfangs-Ringpuffer.
 *
 *  Blockiert, bis ein Byte verfuegbar ist.
 *  Nach JEDEM konsumierten Byte wird ggf. nachgeruestet: Wenn das Endpoint
 *  wegen Ring-Voll geschlossen war, oeffnet es sich genau dann wieder,
 *  wenn wieder >= 64 Bytes Platz sind -- nicht erst, wenn der Ring ganz
 *  leer ist.
 *  Der Billig-Check (Flag lesen) passiert ohne IRQ-Sperre; die teure,
 *  sperrende Variante laeuft nur, wenn tatsaechlich nicht geruestet ist.
 *
 * @param  None
 * @retval Naechstes empfangenes Byte (0..255)
 */
uint8_t byte_io_usb_get(void) {
   uint8_t chr;
   while (!ringbuf_get(&chr)) {
      usb_rx_arm_when_space(); /* Ring leer -> Endpoint ggf. oeffnen */
   }
   if (!rx_armed) {
      usb_rx_arm_when_space(); /* Platz geschaffen -> ggf. oeffnen */
   }
   return chr;
}

/**
 * @brief  Entfernt alle noch ungelesenen Bytes aus dem Empfangskanal und
 *         macht ihn sofort wieder empfangsbereit.
 *         Nur aus dem Anwendungskontext (Consumer-Rolle) aufrufen.
 * @param  None
 * @retval None
 */
void byte_io_usb_clear(void) {
   ringbuf_flush();
   usb_rx_arm_when_space();
}

/**
 * @brief  Sendet ein einzelnes Byte via CDC (busy-sicher, blockierend).
 * @param  byte  Zu sendendes Byte.
 * @retval None
 */
void byte_io_usb_put(uint8_t byte) {
   static uint8_t tx_byte; /* NICHT auf dem Stack: muss den Transfer ueberleben */

   tx_byte = byte;

   while (cdc_tx_busy()) {
   }

   __disable_irq();  // NOLINT(misc-include-cleaner) /* kritische Sektion: Main-Loop vs. USB-ISR */
   if (USBD_OK == cdc_transmit(&tx_byte, 1)) {
      __enable_irq();  // NOLINT(misc-include-cleaner)
      while (cdc_tx_busy()) {
      }
   } else {
      __enable_irq();  // NOLINT(misc-include-cleaner)
   }
}

/* ========================================================================== */
/* Transport-Initialisierung (kaskadierend aus init_input, input.c)          */
/* ========================================================================== */

/**
 * @brief  Initialisiert die Transportschicht: Ringpuffer, USB-Stack und
 *         Diagnosezaehler; setzt danach den Arm-Zustand realitaetstreu.
 *
 *  Reihenfolge:
 *    0. Ringpuffer und Diagnose zuruecksetzen.
 *    1. USB & Stack starten. Der Stack ruestet das OUT-Endpoint selbst auf.
 *    2. Enumerationszeit geben (Host laedt usbmodem-Treiber).
 *    3. Arm-Zustand REALITAETSTREU setzen -- NICHT selbst aufruesten:
 *       USBD_CDC_Init hat das Endpoint bereits scharf geschaltet.
 *
 *  AUFRUFER: ausschliesslich init_input() (input.c) -- die Protokollschicht
 *  kaskadiert hierher, damit die Reihenfolge "erst Transport, dann
 *  Handshake" nicht vom Aufrufer abhaengt.
 *
 * @param  None
 * @retval None
 */
void byte_io_usb_init(void) {
   /* 0. Empfangs-Ringpuffer zuruecksetzen (Speicher liegt in ringbuf.c). */
   ringbuf_init();
   rx_overflow = 0;
   rx_overflow_free_last = 0;
   rx_overflow_lost_last = 0;

   /* 1. USB & Stack starten. Der Stack ruestet das OUT-Endpoint selbst auf. */
   usb_device_init();

   /* 2. Enumerationszeit geben (Host laedt usbmodem-Treiber). */
   HAL_Delay(250);  // NOLINT(readability-magic-numbers)

   /* 3. Arm-Zustand REALITAETSTREU setzen -- NICHT selbst aufruesten:
         USBD_CDC_Init hat das Endpoint bereits scharf geschaltet. */
   rx_armed = true;
}

// EOF