/**
 ******************************************************************************
 * @file    input.h
 * @brief   Oeffentliche API der Protokoll-/Dateischicht des Input-Interfaces:
 *          Handshake mit der PC-Anwendung, dateiweiser Transport
 *          (open_nxt_file/nxt_chr/read_file/eof), Taster-Abfrage am LCD
 *          (ask_transport, show_s5_hint, wait_for_s_i).
 *
 *          Implementiert in input.c. Der Byte-Kanal kommt aus der
 *          Transportschicht (byte_io.h -> byte_io.c -> Backends siehe
 *          byte_io_backend.h); der Transport (USB-CDC vs. UART-via-USB) wird
 *          in init_input() gewaehlt und der PC-Seite ueber 'F' bzw. 'S'
 *          signalisiert.
 ******************************************************************************
 */

#ifndef _INPUT_H
#define _INPUT_H

#include "transport.h"
#include <stdbool.h>
/* EOF (Rueckgabewert von nxt_chr/read_file) kommt aus <stdio.h> -- direkt in
   den nutzenden .c-Dateien inkludieren (input.c, compressedBMP.c) */

/* Taster-Nummern fuer wait_for_s_i(): BUTTON_S5/S6/S7 entsprechen den
   Tastern S5/S6/S7 auf PF5/PF6/PF7 (active-low, Pull-up aus initITSboard) */
#define BUTTON_S5 5
#define BUTTON_S6 6
#define BUTTON_S7 7

/**
 * @brief This function setups communication protocol to Python send file
 *        program. Der Transport wird hier gewaehlt und der PC-Anwendung
 *        ueber das Start-Kommando signalisiert ('F' = USB-CDC mit
 *        USB-Flow-Control, 'S' = UART-via-USB mit 'D'-Burst-Requests).
 * @param transport  TRANSPORT_USB_CDC (Default) oder TRANSPORT_UART_VIA_USB
 * @retval None
 */
extern void init_input(transport_t transport);

/**
 * @brief  Fragt auf dem LCD ab, welcher Transport genutzt werden soll, und
 *         blockiert bis zum Tastendruck: S7 (PF7) = USB-CDC,
 *         S6 (PF6) = UART-via-USB.
 * @param  None
 * @retval TRANSPORT_USB_CDC oder TRANSPORT_UART_VIA_USB
 */
extern transport_t ask_transport(void);

/**
 * @brief  Zeigt einmalig den Hilfetext "S5 (PF5) = weiter / naechstes Bild"
 *         auf dem LCD an. Aufruf nach init_input() (Verbindung steht), direkt
 *         vor dem ersten wait_for_s_i(BUTTON_S5) -- so ist sichergestellt,
 *         dass der Anwender den Text sieht, bevor das erste Bild das Display
 *         loescht.
 * @param  None
 * @retval None
 */
extern void show_s5_hint(void);

/**
 * @brief  Blockiert ohne Textausgabe, bis die Taste S_i vollstaendig
 *         gedrueckt wird (i = BUTTON_S5..BUTTON_S7: S5/S6/S7 auf
 *         PF5/PF6/PF7, active-low).
 *         Wartet erst auf Loslassen, dann auf Druecken -- ein Tastendruck
 *         zaehlt genau einmal, auch bei direkt hintereinanderliegenden
 *         wait_for_s_i()-Aufrufen. Erst danach wird das naechste Bild
 *         angefordert. Der Hilfetext "S5 (PF5) = weiter / naechstes Bild"
 *         erscheint nur einmalig via show_s5_hint() nach Aufbau der
 *         Verbindung.
 * @param  btn  Taster-Nummer: BUTTON_S5/S6/S7 = S5 (PF5) / S6 (PF6) / S7 (PF7)
 * @retval None
 */
extern void wait_for_s_i(int btn);

/**
 * @brief This function asks the Python program to open a new file.
 *        If all files has been read, it blocks until a new file has been
 *        selected in Python program.
 * @param None
 * @retval None
 */
extern void open_nxt_file(void);

/**
 * @brief  This function reads the next byte from the current file that will be
 *         transmitted by Python.
 * @param  None
 * @retval Next byte of the current file or EOF (32 bit value for -1)
 */
extern int nxt_chr(void);

/**
 * @brief  This function reads count elements of data, each size bytes long,
 *         from from the current file that will be transmitted by Python.
 * @param  buf   Storage buffer for data
 * @param  size  Size of each data element
 * @param  count Number of data elements
 * @retval Number of items read. If not enough data are available, EOF will be
 * returned.
 */
extern int read_file(char *buf, unsigned int size, unsigned int count);

/**
 * @brief  Queries whether the current file has been read completely --
 *         WITHOUT consuming a data byte.
 *
 *         If the current burst is exhausted, this function prefetches the
 *         next length header (blocking). The end-of-file state is only
 *         known afterwards. A true result stays valid until
 *         open_nxt_file() starts the next file.
 * @param  None
 * @retval true: end of file reached (EOF marker len == 0 read, or no file
 *         opened yet)
 */
extern bool eof(void);

#endif
// EOF