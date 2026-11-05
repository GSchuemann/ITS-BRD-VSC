/**
 ******************************************************************************
 * @file    byte_io_backend.h
 * @brief   Interne Schnittstelle der Transport-Backends an den Dispatcher
 *          (byte_io.c). Pro Backend genau die vier byte_io-Primitive mit
 *          identischer Semantik; die oeffentliche Seite bleibt byte_io.h.
 *
 *          Backends:
 *            byte_io_usb.c   USB-CDC (OTG_FS, Ringpuffer + USB-Flow-Control)
 *            byte_io_uart.c  UART via USB (USART3 + DMA1-Ringpuffer)
 *
 *          Beide Backends implementieren dasselbe Interface aus vier
 *          Primitiven. Pro Primitive ist unten EIN Doxygen-Block (der
 *          Vertrag) und darunter BEIDE Signaturen -- der Dispatcher
 *          (byte_io.c) ruft je nach gewaehltem transport_t die eine oder
 *          die andere:
 ******************************************************************************
 */

#ifndef BYTE_IO_BACKEND_H
#define BYTE_IO_BACKEND_H

#include <stdint.h>

/**
 * @brief  Kanal des Backends aufsetzen. Blockt ggf. kurz.
 *         Aufruf ausschliesslich aus byte_io_init() (byte_io.c).
 *         USB-CDC: USB-Geraet + Ringpuffer + Endpoint-Arm (blockt wegen
 *         Enumerationszeit). UART: USART3-Baudrate nachziehen (nach
 *         switch_to_168_MHz) + RX per DMA1 Stream1 Ch4 (circular).
 * @param  None
 * @retval None
 */
void byte_io_usb_init(void);  /* Implementierung: byte_io_usb.c */
void byte_io_uart_init(void); /* Implementierung: byte_io_uart.c */

/**
 * @brief  Naechstes Byte blockierend aus dem Empfangskanal des Backends lesen.
 *         USB-CDC: Ringpuffer, Endpoint-Arm nach jedem Byte (USB-Flow-Control).
 *         UART: USART3-DMA-Ringpuffer.
 * @param  None
 * @retval Empfangenes Byte (0..255)
 */
uint8_t byte_io_usb_get(void);  /* Implementierung: byte_io_usb.c */
uint8_t byte_io_uart_get(void); /* Implementierung: byte_io_uart.c */

/**
 * @brief  Ein Byte blockierend ueber den Kanal des Backends senden.
 *         USB-CDC: CDC-Tx, busy-sicher. UART: USART3, polling TXE/TC.
 * @param  byte  Zu sendendes Byte
 * @retval None
 */
void byte_io_usb_put(uint8_t byte);  /* Implementierung: byte_io_usb.c */
void byte_io_uart_put(uint8_t byte); /* Implementierung: byte_io_uart.c */

/**
 * @brief  Alle noch ungelesenen Bytes im Empfangskanal des Backends verwerfen;
 *         Kanal danach sofort wieder empfangsbereit.
 * @param  None
 * @retval None
 */
void byte_io_usb_clear(void);  /* Implementierung: byte_io_usb.c */
void byte_io_uart_clear(void); /* Implementierung: byte_io_uart.c */

#endif /* BYTE_IO_BACKEND_H */
// EOF
