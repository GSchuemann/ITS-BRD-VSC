/**
 ******************************************************************************
 * @file    byte_io.h
 * @brief   Byte-Kanal (Transportschicht) zwischen dem gewaehlten Transport-
 *          Backend (USB-CDC oder UART-via-USB) und der Protokoll-/Dateischicht
 *          (input.c).
 *
 *          Diese Datei ist der einzige sichtbare Teil der Transportschicht:
 *          input.c inkludiert weder USB-Header (Klassen-Handle, USBD_*-Typen)
 *          noch UART-/Ringpuffer-Interna -- die Protokollschicht sieht nur
 *          blockierende Byte-Primitive.
 *
 *          byte_io.c ist ein Dispatcher: byte_io_init(transport) waehlt das
 *          Backend (byte_io_usb.c / byte_io_uart.c, siehe byte_io_backend.h)
 *          und alle Primitive reichen an das gewaehlte Backend durch.
 *
 *          Kaskadierende Initialisierung: byte_io_init() wird nicht von der
 *          Anwendung, sondern ausschliesslich von init_input() (input.c)
 *          aufgerufen. Damit ist die Reihenfolge "erst Transport
 *          initialisieren, dann Protokoll-Handshake" konstruktiv gesichert.
 ******************************************************************************
 */

#ifndef BYTE_IO_H
#define BYTE_IO_H

#include "transport.h"
#include <stdint.h>

/**
 * @brief  Initialisiert die Transportschicht fuer den gewaehlten Kanal:
 *         USB-CDC (Ringpuffer, USB-Geraet, Diagnosezaehler; blockt kurz
 *         wegen der Enumerationszeit) oder UART-via-USB (USART3-DMA-
 *         Ringpuffer).
 *
 *         AUFRUFER: ausschliesslich init_input() (input.c) -- die
 *         kaskadierende Init-Reihenfolge darf nicht umgangen werden.
 * @param  transport  TRANSPORT_USB_CDC (Default) oder TRANSPORT_UART_VIA_USB
 * @retval None
 */
void byte_io_init(transport_t transport);

/**
 * @brief  Liefert den aktuell gewaehlten Transport (von byte_io_init gesetzt).
 *         input.c nutzt dies fuer die Protokoll-Weiche 'S'/'F' und 'D'.
 * @param  None
 * @retval Aktiver Transport
 */
transport_t byte_io_transport(void);

/**
 * @brief  Liest das naechste Byte blockierend aus dem Empfangskanal
 *         (Ringpuffer des gewaehlten Backends). USB-CDC: ruft nach jedem
 *         konsumierten Byte ggf. den Empfangs-Endpoint nach (USB-Flow-Control).
 * @param  None
 * @retval Empfangenes Byte (0..255)
 */
uint8_t byte_io_get(void);

/**
 * @brief  Sendet ein einzelnes Byte blockierend (busy-sicher) ueber den
 *         gewaehlten Kanal (USB-CDC: CDC-Tx; UART: USART3).
 * @param  byte  Zu sendendes Byte
 * @retval None
 */
void byte_io_put(uint8_t byte);

/**
 * @brief  Verwirft alle noch ungelesenen Bytes im Empfangskanal und
 *         macht den Kanal sofort wieder empfangsbereit.
 *         Nur aus dem Anwendungskontext (Consumer-Rolle) aufrufen.
 * @param  None
 * @retval None
 */
void byte_io_clear(void);

#endif /* BYTE_IO_H */
