/*
 * transport.h
 *
 * Wahl des Transportkanals fuer den BITMAP-Input-Pfad. Wird beim Aufruf
 * von init_input(transport) gesetzt und legt fest, welches Protokoll die
 * PC-Anwendung faehrt:
 *
 *   TRANSPORT_USB_CDC      'F' vor jeder Datei, danach blind streamen
 *                          (Flusskontrolle ueber USB-Endpoint-NAK)
 *   TRANSPORT_UART_VIA_USB 'S' vor jeder Datei, danach Burst-Request per
 *                          'D' (UART-via-USB hat keine HW-Flusskontrolle)
 *
 * Die PC-Anwendung erkennt am 'S' bzw. 'F', welcher Modus aktiv ist.
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

typedef enum {
   TRANSPORT_USB_CDC = 0, /* Default */
   TRANSPORT_UART_VIA_USB
} transport_t;

#endif /* TRANSPORT_H */
// EOF
