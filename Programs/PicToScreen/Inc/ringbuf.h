/**
 ******************************************************************************
 * @file    ringbuf.h
 * @brief   SPSC-Ringpuffer (Single Producer / Single Consumer) als Singleton
 *          fuer den USB-CDC-Empfangspfad.
 *
 *          Producer: USB-ISR (ringbuf_write), Consumer: Main-Loop (ringbuf_get).
 *
 *          Der Ringpuffer existiert im gesamten Projekt genau einmal; Speicher,
 *          Verwaltungsdaten und Groesse (RINGBUF_SIZE) liegen als static in
 *          ringbuf.c und sind von aussen nicht erreichbar. Keine Funktion
 *          bekommt Handle-, Speicher- oder Groessenargumente.
 *
 *          Konventionen:
 *            - Nutzbar sind size-1 Bytes (ein Platz fuer Leer/Voll-Logik).
 *            - head gehoert nur dem Producer, tail nur dem Consumer (32-Bit-
 *              Zugriffe sind auf dem Cortex-M atomar; keine kritische Sektion).
 *            - ERST Datenseite schreiben, DANN head veroeffentlichen.
 *
 *          ringbuf_free() ist die einzige Quelle fuer die Arm-Entscheidung des
 *          OUT-Endpoints (byte_io_usb.c: usb_rx_arm_when_space); seine Rechnung
 *          muss mit der Voll-Erkennung in ringbuf_write identisch bleiben --
 *          jede Abweichung fuehrt zu Paket-Teilwrites (rx_overflow).
 ******************************************************************************
 */

#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Initialisierung                                                     */
/* ------------------------------------------------------------------ */

/**
 * Setzt den Ringpuffer zurueck (head = tail = 0).
 * Speicher und Groesse sind im Modul festgelegt; nichts zu pruefen.
 */
void ringbuf_init(void);

/* ------------------------------------------------------------------ */
/* Producer-Seite (ISR-Kontext)                                        */
/* ------------------------------------------------------------------ */

/**
 * Legt einen Block (max. 1 CDC-Paket) im Ring ab.
 * @return Anzahl tatsaechlich geschriebener Bytes (<= len)
 */
uint32_t ringbuf_write(const uint8_t *data, uint32_t len);

/* ------------------------------------------------------------------ */
/* Consumer-Seite (Main-Loop-Kontext)                                  */
/* ------------------------------------------------------------------ */

/** Anzahl aktuell lesbarer Bytes im Ring. */
uint32_t ringbuf_available(void);

/** Freie Bytes im Ring (size-1-Logik, identisch zur Voll-Erkennung
 *  in ringbuf_write). EINZIGE Quelle der USB-Flow-Control-Entscheidung. */
uint32_t ringbuf_free(void);

/**
 * Holt das naechste Byte aus dem Ring.
 * @return true: Byte gelesen; false: Ring leer
 */
bool ringbuf_get(uint8_t *out);

/** Verwirft alle ungelesenen Bytes (tail = head; nur Consumer aufrufen). */
void ringbuf_flush(void);

#endif /* RINGBUF_H */
