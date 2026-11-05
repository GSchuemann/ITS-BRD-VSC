#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include <stdbool.h>

#define EOK                     0
#define INTERNAL_ERR            1 
#define ERR_NO_ERR              2
#define READ_ERR                3
#define HEADER_ERR              4 
#define INV_HEADER_ERR          5 
#define INDEX_ERR               6       
#define COL_TAB_ERR             7  
#define UNSUPPORTED_FORMAT_ERR  8 
#define FORMAT_ERR              9
#define LINE_BUF_ERR           10
#define IMG_SIZE_ERR           11

// Do not call this function directly. Use the wrapper macros CHECK / CHECK_FATAL.
extern int my_err_handler(bool cnd, char *file, int line, int err_no, bool loop_for_ever);

/**
 * @brief Wrapper-Makros des einfachen Error Handlers my_err_handler, der
 *        eine Fehlermeldung auf dem LCD-Display ausgibt. Alle Makros
 *        verwenden eine einheitliche Invarianten-Semantik wie assert:
 *        Die Bedingung muss wahr sein, sonst wird der Fehler gemeldet.
 *
 *        Zwei Grundbausteine:
 *        CHECK       wertet die Bedingung aus und liefert EOK bzw. err_no
 *                    als Ausdruck zurueck (z. B. return CHECK(cnd, err)).
 *        RETURN_ON_ERROR ist das Aequivalent zum Rust-?-Operator: Liefert
 *                    der Aufruf einen Fehlercode (alles != EOK), wird
 *                    dieser an den Aufrufer weitergereicht.
 *        CHECK_FATAL meldet den Fehler und bleibt danach in einer
 *                    Endlosschleife (fuer verletzte Invarianten).
 *
 *        CHECK_OR_RETURN ist die Komposition beider Bausteine
 *                    (Anweisungsform: Bedingung falsch -> return err_no).
 * @param cnd  If this condition is false, an error will be reported.
 * @retval None
 */

#define CHECK(cnd, err_no)       my_err_handler((cnd), __FILE__, __LINE__, (err_no), false)
#define CHECK_FATAL(cnd, err_no) my_err_handler((cnd), __FILE__, __LINE__, (err_no), true)

// Als do-while(0) gekapselt, damit das Makro wie eine einzelne Anweisung
// funktioniert (sicher in if/else ohne umgebende Klammern).
#define RETURN_ON_ERROR(fcall) \
   do { \
      int erg = (fcall); \
      if (EOK != erg) { return erg; } \
   } while (0)

// Komposition aus CHECK und RETURN_ON_ERROR: Bedingung wahr = OK, sonst
// Fehler melden und err_no an den Aufrufer zurueckgeben.
#define CHECK_OR_RETURN(cnd, err_no) RETURN_ON_ERROR(CHECK(cnd, err_no))

#endif
// EOF
