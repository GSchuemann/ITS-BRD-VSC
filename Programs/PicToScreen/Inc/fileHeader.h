/**
 ******************************************************************************
 * @file    fileHeader.h
 * @brief   Gekapselte Einstiegsfunktion der Input-Verarbeitung: Datei vom PC
 *          einlesen, BITMAP-Header testen und beide Header (BITMAPFILEHEADER
 *          + BITMAPINFOHEADER) auf dem LCD-Display ausgeben.
 *
 *          Modul entkoppelt, damit input / headers / errorhandler den
 *          Studierenden separat zur Verfuegung gestellt werden koennen
 *          (siehe Docs/).
 ******************************************************************************
 */

#ifndef _FILEHEADER_H
#define _FILEHEADER_H

/**
 * @brief  Wird ganz am Anfang der Input-Verarbeitung aufgerufen (zu Beginn
 *         von handle_picture() in bmpApp.c):
 *           1. naechste Datei vom PC oeffnen      (open_nxt_file, input.h)
 *           2. BITMAP-Header einlesen und testen  (read_hdrs, headers.h)
 *           3. beide Header (BITMAPFILEHEADER + BITMAPINFOHEADER) auf dem
 *              LCD-Display ausgeben; danach ohne Textausgabe auf S5 (PF5)
 *              warten (wait_for_s_i(5)), dann Display loeschen -- erst dann
 *              folgt die Bilddarstellung
 * @param  None
 * @retval EOK (errorhandler.h) oder Fehlercode (READ_ERR, HEADER_ERR,
 *         INV_HEADER_ERR, ...)
 */
extern int input_file_header(void);

#endif /* _FILEHEADER_H */
// EOF
