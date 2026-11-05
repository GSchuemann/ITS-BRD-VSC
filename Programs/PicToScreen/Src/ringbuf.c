/**
 ******************************************************************************
 * @file    ringbuf.c
 * @brief   Implementierung des SPSC-Ringpuffers als Singleton, siehe ringbuf.h.
 *          Instanz und Backing-Speicher liegen hier -- die Aussenwelt sieht nur
 *          die parameterlosen Funktionen aus ringbuf.h.
 *
 *          Wichtige Reihenfolge beim Schreiben eines Bytes:
 *            ERST Datenseite schreiben, DANN head aktualisieren.
 *          Damit sieht der Consumer ein neues Byte erst zusammen mit dem
 *          aktualisierten Index (kein "Index zeigt hin, Datum fehlt"-Zustand).
 *          Der Cortex-M4 hat einen stark geordneten Speicher; ein zusaetzliches
 *          __DMB() ist daher nicht zwingend, schadet aber auch nicht.
 ******************************************************************************
 */

#include "ringbuf.h"

/* ------------------------------------------------------------------ */
/* Die EINZIGE Instanz: Speicher + Verwaltungsdaten im Modul          */
/* ------------------------------------------------------------------ */

/** Groesse des Ringpuffers in Bytes (Implementierungsdetail).
 *  MUSS eine Zweierpotenz >= 2 sein -- Compile-Zeit-Sicherung unten. */
#define RINGBUF_SIZE 2048U

/* Compile-Zeit-Sicherung: genau ein gesetztes Bit (C99 ohne <assert.h>). */
#if (RINGBUF_SIZE < 2U) || ((RINGBUF_SIZE & (RINGBUF_SIZE - 1U)) != 0U)
#error "RINGBUF_SIZE muss eine Zweierpotenz >= 2 sein"
#endif

static volatile uint32_t head; /* Producer-Index (naechster Schreibplatz) */
static volatile uint32_t tail; /* Consumer-Index (naechster Leseplatz)   */
static uint8_t storage[RINGBUF_SIZE];
#define RINGBUF_MASK (RINGBUF_SIZE - 1U) /* schnelle Modulo-Logik */

/* ------------------------------------------------------------------ */
/* Initialisierung                                                     */
/* ------------------------------------------------------------------ */

void ringbuf_init(void) {
   head = 0U;
   tail = 0U;
}

/* ------------------------------------------------------------------ */
/* Producer-Seite (ISR-Kontext)                                        */
/* ------------------------------------------------------------------ */

/** Legt EIN Byte im Ring ab (nur modulintern).
 *  @return true: geschrieben; false: Ring voll (Byte verworfen) */
static bool ringbuf_put(uint8_t byte) {
   uint32_t next = (head + 1U) & RINGBUF_MASK;

   if (next == tail) {
      return false; /* voll: Byte verwerfen */
   }

   storage[head] = byte; /* 1. Datum schreiben  */
   head = next;          /* 2. Index veroeffentlichen */
   return true;
}

/* Wird von cdc_collect() (byte_io.c) mit einem CDC-Paket (max. 64 Bytes)
 * aufgerufen. Der Rueckgabewert entscheidet ueber die Diagnose:
 * written < len heisst Ring war (teil-)voll -- bei aktivierter
 * USB-Flow-Control ein Protokoll-/Registerfehler, kein normaler Zustand. */
uint32_t ringbuf_write(const uint8_t *data, uint32_t len) {
   uint32_t written = 0U;

   while (written < len) {
      if (!ringbuf_put(data[written])) {
         break; /* voll: Rest verwerfen */
      }
      written++;
   }
   return written;
}

/* ------------------------------------------------------------------ */
/* Consumer-Seite (Main-Loop-Kontext)                                  */
/* ------------------------------------------------------------------ */

uint32_t ringbuf_available(void) { return (head - tail) & RINGBUF_MASK; }

uint32_t ringbuf_free(void) {
   /* Nutzbar sind size-1 Bytes (ein Platz fuer die Leer/Voll-Logik).
      Muss identisch zur Voll-Erkennung in ringbuf_put bleiben:
      dort ist voll, wenn (head+1)&mask == tail, d. h. wenn available
      == size-1 -> free == 0. */
   return (RINGBUF_SIZE - 1U) - ringbuf_available();
}

bool ringbuf_get(uint8_t *out) {
   if (tail == head) {
      return false; /* leer */
   }

   *out = storage[tail];
   tail = (tail + 1U) & RINGBUF_MASK;
   return true;
}

void ringbuf_flush(void) {
   /* Nur Consumer darf tail schreiben: tail = head ist atomar
      (32-Bit-Zugriff auf dem Cortex-M) und braucht keine Sperre.   */
   tail = head;
}
