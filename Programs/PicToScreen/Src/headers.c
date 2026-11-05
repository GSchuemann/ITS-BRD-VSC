/**
 ******************************************************************************
 * @file    headers.c
 * @brief   Implementierung von headers.h: Einlesen der BITMAP-Header
 *          (BITMAPFILEHEADER + BITMAPINFOHEADER) aus dem Eingabestrom des PC
 *          (read_file(), input.h) in statische Zwischenpuffer, Pruefung via
 *          basic_checks() und Auslieferung ueber get_file_hdr() /
 *          get_info_hdr().
 ******************************************************************************
 */

#include "headers.h"
#include "errorhandler.h"
#include "input.h"
#include <stdbool.h>

static BITMAPFILEHEADER file_hdr;
static BITMAPINFOHEADER info_hdr;
static bool hdrs_valid = false;

/**
 * @brief  Prueft den BITMAPFILEHEADER: Signatur ('BM') und bfOffBits muss
 *         innerhalb der Datei liegen (bfOffBits < bfSize).
 * @param  file_hdr_p  Zu pruefender Header (nie NULL; static, einzige
 *                     Aufrufstelle ist basic_checks() mit &file_hdr)
 * @retval EOK oder HEADER_ERR
 */
static int check_file_hdr(const BITMAPFILEHEADER *file_hdr_p) {
   CHECK_OR_RETURN(file_hdr_p->bfType == BMP_SIGNATURE, HEADER_ERR); // Wrong signature.
   CHECK_OR_RETURN(file_hdr_p->bfOffBits < file_hdr_p->bfSize, HEADER_ERR); // bfOffBits points out of BMP file
   return EOK;
}

/**
 * @brief  Prueft die allgemeinen Felder des BITMAPINFOHEADER: biSize,
 *         biPlanes == 1 sowie positive Ausdehnung (biWidth, biHeight >= 1).
 * @param  info_hdr_p  Zu pruefender Header (nie NULL; static, einzige
 *                     Aufrufstelle ist basic_checks() mit &info_hdr)
 * @retval EOK oder HEADER_ERR
 */
static int check_info_hdr_dims(const BITMAPINFOHEADER *info_hdr_p) {
   CHECK_OR_RETURN(info_hdr_p->biSize == sizeof(BITMAPINFOHEADER), HEADER_ERR); // Wrong biSize
   CHECK_OR_RETURN(info_hdr_p->biPlanes == 1, HEADER_ERR); // Wrong biPlanes - must be 1
   CHECK_OR_RETURN(1 <= info_hdr_p->biHeight, HEADER_ERR); // biHeight must be positive
   CHECK_OR_RETURN(1 <= info_hdr_p->biWidth, HEADER_ERR); // biWidth must be positive
   return EOK;
}

/**
 * @brief  Prueft Bittiefe, Kompression und Farbtabellen-Groesse des
 *         BITMAPINFOHEADER: biBitCount 8 oder 24, bei 24 bpp nur BI_RGB
 *         ( unkomprimiert), sonst BI_RLE8 oder BI_RGB, biClrUsed <=
 *         MAX_COLOR_TABLE_SIZE und bei 24 bpp biClrUsed == 0.
 * @param  info_hdr_p  Zu pruefender Header (nie NULL; static, einzige
 *                     Aufrufstelle ist basic_checks() mit &info_hdr)
 * @retval EOK oder HEADER_ERR
 */
static int check_info_hdr_format(const BITMAPINFOHEADER *info_hdr_p) {
   CHECK_OR_RETURN((8 == info_hdr_p->biBitCount) || (24 == info_hdr_p->biBitCount), HEADER_ERR); // Wrong biBitCount, 8 or 24 expected
   CHECK_OR_RETURN((24 != info_hdr_p->biBitCount) || (BI_RGB == info_hdr_p->biCompression), HEADER_ERR); // 24 bit format cannot be compressed
   CHECK_OR_RETURN((BI_RLE8 == info_hdr_p->biCompression) || (BI_RGB == info_hdr_p->biCompression), HEADER_ERR); // unexpected compression format
   CHECK_OR_RETURN(MAX_COLOR_TABLE_SIZE >= info_hdr_p->biClrUsed, HEADER_ERR); // biClrUsed out of range
   CHECK_OR_RETURN((24 != info_hdr_p->biBitCount) || (0 == info_hdr_p->biClrUsed), HEADER_ERR); // biClrUsed must be 0 for 24 bit/pixel format
   return EOK;
}

/**
 * @brief  Prueft beide modul-globalen Header (file_hdr, info_hdr) in fester
 *         Reihenfolge: erst check_file_hdr(), dann check_info_hdr_dims(),
 *         dann check_info_hdr_format(). Erster Fehler wird durchgereicht.
 * @param  None
 * @retval EOK oder HEADER_ERR
 */
static int basic_checks(void) {
   RETURN_ON_ERROR(check_file_hdr(&file_hdr));
   RETURN_ON_ERROR(check_info_hdr_dims(&info_hdr));
   RETURN_ON_ERROR(check_info_hdr_format(&info_hdr));
   return EOK;
}

int read_hdrs(void) {
   hdrs_valid = false;
   CHECK_OR_RETURN(1 == read_file((char *)&file_hdr, sizeof(BITMAPFILEHEADER), 1), READ_ERR);
   CHECK_OR_RETURN(1 == read_file((char *)&info_hdr, sizeof(BITMAPINFOHEADER), 1), READ_ERR);
   int erg = basic_checks();
   hdrs_valid = EOK == erg;
   return erg;
}

int get_file_hdr(BITMAPFILEHEADER *file_hdr_p) {
   RETURN_ON_ERROR(CHECK(hdrs_valid, INV_HEADER_ERR));
   *file_hdr_p = file_hdr;
   return EOK;
}

int get_info_hdr(BITMAPINFOHEADER *info_hdr_p) {
   RETURN_ON_ERROR(CHECK(hdrs_valid, INV_HEADER_ERR));
   *info_hdr_p = info_hdr;
   return EOK;
}
// EOF
