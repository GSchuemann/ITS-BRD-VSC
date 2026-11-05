/*
 * cdc_input_if.h
 *
 * Mini-Schnittstelle zwischen USB-CDC-Middleware (usbd_cdc_if.c) und
 * Transportschicht (byte_io.c). Bewusst OHNE USB-Header: Beide Seiten includieren
 * nur dieses File -- die Transportschicht sieht keine USB-Interna (Klassen-
 * Handle, USBD_*-Typen), und die Middleware keine Applikationsdetails.
 *
 * Zwei Richtungen:
 *   - Hooks (Middleware ruft, byte_io.c implementiert):
 *       cdc_collect(), usb_rx_arm_when_space()
 *   - Wrapper (Anwendung ruft, usbd_cdc_if.c implementiert):
 *       cdc_rx_arm(), cdc_tx_busy(), cdc_transmit()
 *
 * Rx-Disziplin (Flow-Control, siehe PROJECT.md Abschnitt 3):
 *   usb_rx_arm_when_space() bleibt die EINZIGE Oeffnen-Entscheidung
 *   (rx_armed + ringbuf_free unter IRQ-Sperre); der reine Hardware-Aufruf
 *   USBD_CDC_ReceivePacket() steckt hinter cdc_rx_arm().
 */

#ifndef CDC_INPUT_IF_H
#define CDC_INPUT_IF_H

#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Hooks: Middleware -> Anwendung                                       */
/* (usbd_cdc_if.c ruft, byte_io.c implementiert)                       */
/* ------------------------------------------------------------------ */

/**
 * Legt ein empfangenes CDC-Paket im Empfangs-Ringpuffer ab.
 * Aufrufer: cdc_receive_fs() (usbd_cdc_if.c, Interrupt-Kontext).
 * @param data  Zeiger auf die empfangenen Bytes
 * @param len   Anzahl der Bytes (max. 64 je CDC-Paket)
 */
void cdc_collect(uint8_t *data, uint32_t len);

/**
 * Oeffnet das USB-OUT-Endpoint wieder, wenn der Ringpuffer Platz fuer
 * ein volles CDC-Paket (64 Bytes) hat (Entscheidung + Hardware-Aufruf
 * atomar unter IRQ-Sperre).
 *
 * Nach dem Empfang eines 64-Byte-Pakets ist das Endpoint geschlossen;
 * es muss explizit wieder geoeffnet werden, sobald das Paket in den
 * Ringpuffer verschoben wurde -- genau das tut diese Funktion.
 * Aufrufer: cdc_receive_fs() (ISR) und Main-Loop (byte_io_get/byte_io_clear).
 */
void usb_rx_arm_when_space(void);

/* ------------------------------------------------------------------ */
/* Wrapper: Anwendung -> Middleware                                    */
/* (byte_io.c ruft, usbd_cdc_if.c implementiert)                       */
/* ------------------------------------------------------------------ */

/**
 * Schaltet das OUT-Endpoint wieder scharf (USBD_CDC_ReceivePacket).
 * @return true: USBD_OK (Endpoint aufgeruestet); false: HAL abgelehnt
 */
bool cdc_rx_arm(void);

/**
 * true, solange ein Tx-Transfer laeuft (TxState != 0).
 * Anwendung pollt dies vor/nach cdc_transmit() (busy-sicher senden).
 */
bool cdc_tx_busy(void);

/**
 * Startet einen Tx-Transfer zum Host (cdc_transmit_fs: prueft
 * dev_state CONFIGURED, setzt TxBuffer/TxLength).
 * @param buf  Zu sendende Bytes (muessen den Transfer ueberleben)
 * @param len  Anzahl der Bytes
 * @return USBD_OK oder USBD_FAIL
 */
uint8_t cdc_transmit(uint8_t *buf, uint16_t len);

#endif /* CDC_INPUT_IF_H */
