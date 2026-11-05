/**
 ******************************************************************************
 * @file    headers.h
 * @brief   Einlesen und Pruefen der BITMAP-Header (BITMAPFILEHEADER +
 *          BITMAPINFOHEADER) aus dem Eingabestrom des PC.
 *
 *          read_hdrs() liest beide Header per read_file() (input.h) in
 *          modul-interne statische Puffer und prueft sie (basic_checks(),
 *          nur in headers.c sichtbar). get_file_hdr() / get_info_hdr()
 *          kopieren einen geprueften Header in den Puffer des Aufrufers --
 *          nur wenn die Header gueltig sind, sonst INV_HEADER_ERR (der
 *          Zielpuffer bleibt dann unberuehrt).
 *
 *          Wird den Studierenden als fertiges Modul zur Verfuegung gestellt
 *          (Nutzung siehe fileHeader.c / Docs/).
 ******************************************************************************
 */

#ifndef _HEADERS_H
#define _HEADERS_H

#include "BMP_types.h"

/**
 * @brief  Liest BITMAPFILEHEADER und BITMAPINFOHEADER der naechsten Datei aus
 *         dem Eingabestrom und prueft sie (Signatur, bfOffBits, biSize,
 *         biPlanes, biWidth/biHeight, biBitCount, biCompression, biClrUsed).
 *         Setzt das modul-interne Gueltigkeits-Flag; erst danach liefern
 *         get_file_hdr() / get_info_hdr() die Header aus.
 * @param  None
 * @retval EOK bei gueltigen Headern, sonst READ_ERR (Lesefehler) oder
 *         HEADER_ERR (Header ungueltig)
 */
extern int read_hdrs(void);

/**
 * @brief  Kopiert den geprueften BITMAPFILEHEADER in den Puffer des Aufrufers.
 * @param  file_hdr_p  Ziel-Puffer; erst nach erfolgreichem read_hdrs()
 *                     auswerten (bei Fehler bleibt er unberuehrt)
 * @retval EOK oder INV_HEADER_ERR, falls die Header nicht gueltig sind
 */
extern int get_file_hdr(BITMAPFILEHEADER *file_hdr_p);

/**
 * @brief  Kopiert den geprueften BITMAPINFOHEADER in den Puffer des Aufrufers.
 * @param  info_hdr_p  Ziel-Puffer; erst nach erfolgreichem read_hdrs()
 *                     auswerten (bei Fehler bleibt er unberuehrt)
 * @retval EOK oder INV_HEADER_ERR, falls die Header nicht gueltig sind
 */
extern int get_info_hdr(BITMAPINFOHEADER *info_hdr_p);
#endif
// EOF
