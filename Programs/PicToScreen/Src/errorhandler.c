#include <errorhandler.h>

#include "lcd.h"
#include <stdio.h>

#define BUF_SIZE 256
static char buf[BUF_SIZE];

static const char *const SYS_ERRLIST[] = {
    [EOK]                     = "Success",
    [INTERNAL_ERR]            = "Internal error", 
    [ERR_NO_ERR]              = "Wrong error number",
    [READ_ERR]                = "Read error",
    [HEADER_ERR]              = "Error in BITMAP header",
    [INV_HEADER_ERR]          = "Invalid BITMAP header",
    [INDEX_ERR]               = "Invalid index",
    [COL_TAB_ERR]             = "Invalid color table",
    [UNSUPPORTED_FORMAT_ERR]  = "Unsupported format",
    [FORMAT_ERR]              = "Error in format",
    [LINE_BUF_ERR]            = "Error in line buffer",
    [IMG_SIZE_ERR]            = "Image size exceeds the display limit"
};

/**
* @brief Simple error handler that writes an error msg on
*        LCD display and loops forever or returns EOK / NOK.
*        To receive correct file name and line number, this
*        function should be called using der wrapper macros
*        CHECK / CHECK_FATAL.
* @param cnd  If this condition is false, an error will be reported.
* @param file The file containing the current call of this function.
* @param line Line number of the current call of this function
* @param msg  Error message
* @param loopForEver Don't return from this function call

* @retval None
*/

int my_err_handler(bool cnd, char *file, int line, int err_no, bool loop_for_ever) {
   if (!cnd) {
      snprintf(buf, sizeof(buf) / sizeof(buf[0]), "Error in file %s line %d: %s\n", file, line, SYS_ERRLIST[err_no]);
      lcdGotoXY(1, 1);
      lcdPrintS(buf);
      while (loop_for_ever) { ; } // NOLINT(bugprone-infinite-loop)
      return err_no;
   }
   return EOK;
}
// EOF
