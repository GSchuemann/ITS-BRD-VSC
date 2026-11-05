/**
 ******************************************************************************
 * @file    main.c  --  Starthilfe zur BITMAP-Uebung
 * @brief   Rahmenprogramm, das den Studierenden als Starthilfe uebergeben wird. 
 *
 *          Diese Version leistet bereits:
 *            1. Transport waehlen (S7/PF7 = USB-CDC, S6/PF6 = UART-via-USB)
 *               und initialisieren: ask_transport() + init_input(); danach
 *               Hilfetext (show_s5_hint()) und Warten auf S5 (PF5), bevor das
 *               erste Bild uebertragen wird
 *            2. BITMAP-Header der naechsten Datei einlesen, pruefen und die
 *               Infos auf dem LCD-Display ausgeben: input_file_header()
 *               aus fileHeader.c 
 *            3. danach wird per Tastendruck S5 (PF5) das naechste Bild
 *               angefordert -- das Warten auf S5 passiert in
 *               input_file_header() (wait_for_s_i(5)), ein Tastendruck
 *               (egal wie lange) genuegt; die naechste Schleifeniteration
 *               liest das Bild ueber open_nxt_file() ein
 *
 ******************************************************************************
 */

#include "LCD_GUI.h"
#include "LCD_Touch.h"
#include "error.h"
#include "errorhandler.h" /* EOK */
#include "fileHeader.h"   /* input_file_header */
#include "fontsFLASH.h"
#include "init.h"
#include "input.h"        /* ask_transport, init_input */
#include "usb_clock_config.h"
#include <timer.h>

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {
   initITSboard();       // Initialisierung des ITS Boards
   switch_to_168_MHz();  // Zur Unterstützung von USB wird die Taktrate auf ein vielfaches von 42 MHz gesetzt
   initTimer();
   GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
   TP_Init(false);                     // Initialisierung des LCD Boards mit Touch
   if (!checkVersionFlashFonts()) {
      Error_Handler();
   }

   transport_t transport = ask_transport(); // Anwender waehlt am LCD: S7 (PF7) = USB-CDC, S6 (PF6) = UART-via-USB
   init_input(transport); // Verbindung aufgebaut (Handshake H/R/G)

   /* Hilfetext einmalig anzeigen und auf S5 warten, sobald die Verbindung
      steht -- erst dann das erste Bild einlesen. */
   show_s5_hint();
   wait_for_s_i(BUTTON_S5); // erst S5 (PF5) startet die Uebertragung des ersten Bilds

   while (1) {
      /* Naechste Datei oeffnen, BITMAP-Header einlesen + pruefen und auf dem
         Display ausgeben; danach wartet input_file_header() selbst ohne
         Textausgabe auf S5 (PF5) und loescht dann das Display -- ein
         Tastendruck (egal wie lange) genuegt fuer den naechsten Header. */
      if (EOK != input_file_header()) {
         /* Fehler wurde bereits im Error-Handler auf dem LCD gemeldet und
            die laufende Operation abgebrochen; Meldung stehen lassen, erst
            S5 (PF5) fordert die naechste Datei an. */
         wait_for_s_i(BUTTON_S5);
      }
   }
}
// EOF
