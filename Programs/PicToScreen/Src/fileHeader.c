/**
 ******************************************************************************
 * @file    fileHeader.c
 * @brief   Gekapselte Einstiegsfunktion der Input-Verarbeitung
 *          (Implementierung von fileHeader.h).
 *
 *          input_file_header() wird ganz am Anfang der Behandlung eines
 *          Bilds aufgerufen und fuehrt folgende Aufgaben aus:
 *            1. naechste Datei vom PC oeffnen       (open_nxt_file, input.h)
 *            2. BITMAP-Header einlesen und testen   (read_hdrs, headers.h)
 *            3. beide Header (BITMAPFILEHEADER + BITMAPINFOHEADER) auf dem
 *               LCD-Display ausgeben; danach ohne Textausgabe auf S5 (PF5)
 *               warten (wait_for_s_i), dann Display loeschen -- erst dann
 *               folgt die Darstellung des Bildes
 ******************************************************************************
 */

#include "fileHeader.h"
#include "BMP_types.h"
#include "errorhandler.h"
#include "headers.h"
#include "input.h"
#include "LCD_GUI.h"
#include "lcd.h"
#include <stdint.h>
#include <stdio.h>

/* Groesse des Zeilenpuffers fuer die Header-Ausgabe auf dem LCD */
#define LINE_BUF_SIZE 48

/* Schiebeweite fuer das High-Byte eines 16-bit-Worts (bfType) */
#define HIGH_BYTE_SHIFT 8

/*
 * Nutzbare LCD-Zeilen der Header-Ausgabe (Default-Font 16 px: lcdGotoXY(1,y)
 * belegt Pixel y*16..y*16+15, GUI_drawRectangle braucht br.y < 320, daher sind
 * nur die Zeilen 1-18 nutzbar): File-Header Titel + 4 Felder (1-5), Leerzeile
 * 6, Info-Header Titel + 10 Felder (7-18).
 */
enum HdrRow {
   ROW_FILE_HDR_TITLE = 1,
   ROW_BFTYPE,
   ROW_BFSIZE,
   ROW_BFRESERVED,
   ROW_BFOFFBITS,
   ROW_EMPTY, /* Leerzeile zwischen den beiden Headern */
   ROW_INFO_HDR_TITLE,
   ROW_BISIZE,
   ROW_BIWIDTH,
   ROW_BIHEIGHT,
   ROW_BIPLANES,
   ROW_BIBITCOUNT,
   ROW_BICOMPRESSION,
   ROW_BISIZEIMAGE,
   ROW_BIXPELSPERMETER,
   ROW_BIYPELSPERMETER,
   ROW_BICLRUSED,
   ROW_BICLRIMPORTANT
};

/**
 * @brief  Gibt beide BITMAP-Header (BITMAPFILEHEADER und BITMAPINFOHEADER)
 *         auf dem LCD-Display aus (Default-Font 16 px: lcdGotoXY(1,y) belegt
 *         Pixel y*16..y*16+15, GUI_drawRectangle braucht br.y < 320, daher
 *         sind nur die Zeilen 1-18 nutzbar: File-Header Zeile 1-5,
 *         Leerzeile 6, Info-Header Zeile 7-18). Danach wird ohne Textausgabe
 *         auf S5 (PF5) gewartet; erst dann Bildschirm loeschen + Cursor auf
 *         Anfang -- zwischen Header-Ausgabe und Bilddarstellung wird nicht
 *         mit Delay gewartet.
 * @param  file_hdr_p  Zeiger auf den gelesenen BITMAPFILEHEADER
 * @param  info_hdr_p  Zeiger auf den gelesenen BITMAPINFOHEADER
 * @retval None
 */
static void show_file_hdr(const BITMAPFILEHEADER *file_hdr_p, const BITMAPINFOHEADER *info_hdr_p) {
   char buf[LINE_BUF_SIZE];
   lcdGotoXY(1, ROW_FILE_HDR_TITLE);
   lcdPrintS("BITMAP FILE HEADER");
   lcdGotoXY(1, ROW_BFTYPE);
   /* bfType little-endian: niederwertiges Byte zuerst ('B'), dann 'M' */
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "bfType:      %c%c (0x%04X)",
            (char)(uint8_t)file_hdr_p->bfType,
            (char)(uint8_t)(file_hdr_p->bfType >> HIGH_BYTE_SHIFT), (unsigned int)file_hdr_p->bfType);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BFSIZE);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "bfSize:      %lu", (unsigned long)file_hdr_p->bfSize);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BFRESERVED);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "bfReserved1/2: %u, %u",
            (unsigned int)file_hdr_p->bfReserved1, (unsigned int)file_hdr_p->bfReserved2);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BFOFFBITS);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "bfOffBits:   %lu", (unsigned long)file_hdr_p->bfOffBits);
   lcdPrintS(buf);
   /* ROW_EMPTY bleibt als Leerzeile zwischen den beiden Headern frei */

   lcdGotoXY(1, ROW_INFO_HDR_TITLE);
   lcdPrintS("BITMAP INFO HEADER");
   lcdGotoXY(1, ROW_BISIZE);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biSize:          %lu", (unsigned long)info_hdr_p->biSize);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BIWIDTH);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biWidth:         %ld", (long)info_hdr_p->biWidth);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BIHEIGHT);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biHeight:        %ld", (long)info_hdr_p->biHeight);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BIPLANES);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biPlanes:        %u", (unsigned int)info_hdr_p->biPlanes);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BIBITCOUNT);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biBitCount:      %u", (unsigned int)info_hdr_p->biBitCount);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BICOMPRESSION);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biCompression:   %lu", (unsigned long)info_hdr_p->biCompression);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BISIZEIMAGE);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biSizeImage:     %lu", (unsigned long)info_hdr_p->biSizeImage);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BIXPELSPERMETER);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biXPelsPerMeter: %ld", (long)info_hdr_p->biXPelsPerMeter);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BIYPELSPERMETER);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biYPelsPerMeter: %ld", (long)info_hdr_p->biYPelsPerMeter);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BICLRUSED);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biClrUsed:       %lu", (unsigned long)info_hdr_p->biClrUsed);
   lcdPrintS(buf);
   lcdGotoXY(1, ROW_BICLRIMPORTANT);
   snprintf(buf, sizeof(buf) / sizeof(buf[0]), "biClrImportant:  %lu", (unsigned long)info_hdr_p->biClrImportant);
   lcdPrintS(buf);

   wait_for_s_i(BUTTON_S5);          /* ohne Textausgabe auf S5 (PF5) warten */
   GUI_clear(BLACK);  /* Bildschirm wieder loeschen ... */
   lcdGotoXY(1, ROW_FILE_HDR_TITLE); /* ... und LCD-Ausgabe auf den Anfang setzen */
}

/**
 * @brief  Siehe fileHeader.h -- oeffnet die naechste Datei, liest und testet
 *         die BITMAP-Header und gibt beide Header auf dem Display aus.
 * @param  None
 * @retval EOK oder Fehlercode
 */
int input_file_header(void) {
   open_nxt_file(); /* naechste Datei vom PC anfordern */
   RETURN_ON_ERROR(read_hdrs()); /* Header einlesen + testen (Modul headers) */
   BITMAPFILEHEADER file_hdr;
   BITMAPINFOHEADER info_hdr;
   RETURN_ON_ERROR(get_file_hdr(&file_hdr)); /* ersten Header holen */
   RETURN_ON_ERROR(get_info_hdr(&info_hdr)); /* zweiten Header holen */
   show_file_hdr(&file_hdr, &info_hdr);
   return EOK;
}

// EOF
