/**
 ******************************************************************************
 * @file    byte_io.c
 * @brief   Transport-Dispatcher (Transportschicht des Input-Pfads):
 *          implementiert byte_io.h und reicht die oeffentlichen Primitive an
 *          das zur Laufzeit gewaehlte Backend durch:
 *
 *            TRANSPORT_USB_CDC       -> byte_io_usb.c  (USB-CDC, OTG_FS)
 *            TRANSPORT_UART_VIA_USB  -> byte_io_uart.c (USART3 + DMA1-Ringpuffer)
 *
 *          Die Protokoll-/Dateischicht (input.c) sieht davon nur byte_io.h.
 *          Welcher Transport aktiv ist, meldet byte_io_transport() -- input.c
 *          signalisiert der PC-Anwendung den Modus ueber 'S' (UART) bzw. 'F' (USB).
 *
 *          AUFRUFER von byte_io_init(): ausschliesslich init_input() (input.c).
 ******************************************************************************
 */

#include "byte_io.h"
#include "byte_io_backend.h"

static transport_t current_transport = TRANSPORT_USB_CDC; /* Default */

void byte_io_init(transport_t transport) {
   current_transport =
       (transport == TRANSPORT_UART_VIA_USB) ? TRANSPORT_UART_VIA_USB : TRANSPORT_USB_CDC;
   if (current_transport == TRANSPORT_UART_VIA_USB) {
      byte_io_uart_init();
   } else {
      byte_io_usb_init();
   }
}

transport_t byte_io_transport(void) {
   return current_transport;
}

uint8_t byte_io_get(void) {
   if (current_transport == TRANSPORT_UART_VIA_USB) {
      return byte_io_uart_get();
   }
   return byte_io_usb_get();
}

void byte_io_put(uint8_t byte) {
   if (current_transport == TRANSPORT_UART_VIA_USB) {
      byte_io_uart_put(byte);
   } else {
      byte_io_usb_put(byte);
   }
}

void byte_io_clear(void) {
   if (current_transport == TRANSPORT_UART_VIA_USB) {
      byte_io_uart_clear();
   } else {
      byte_io_usb_clear();
   }
}

// EOF
