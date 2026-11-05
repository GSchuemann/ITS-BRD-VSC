/**
 ******************************************************************************
 * @file    usbd_cdc_if.c
 * @brief   CDC-Anwendungsadapter (Implementierung von usbd_cdc_if.h): CDC-
 *          Klasse an die Anwendung anbinden -- fops-Struct (Init/Receive/
 *          TransmitCplt), Empfang an cdc_collect() (byte_io.c) weiterreichen
 *          und die Projekt-Wrapper cdc_rx_arm/cdc_tx_busy/cdc_transmit
 *          bereitstellen.
 ******************************************************************************
 */
#include "usbd_cdc_if.h"
#include "usb_device.h" /* usbd_fs */
#include "cdc_input_if.h"
#include "usbd_cdc.h"
#include "usbd_def.h" /* USBD_OK, USBD_FAIL, USBD_STATE_CONFIGURED */
#include "stm32f4xx_hal_def.h" /* UNUSED (hal.h ist ueber usb_device.h schon geladen) */
#include <stdbool.h>
#include <stddef.h> /* NULL */
#include <stdint.h>

/* --- Deklarationen --- */
static int8_t cdc_init_fs(void);
static int8_t cdc_deinit_fs(void);
static int8_t cdc_control_fs(uint8_t cmd, uint8_t *pbuf, uint16_t len);  // NOLINT(readability-non-const-parameter)
static int8_t cdc_receive_fs(uint8_t *buf, uint32_t *len);  // NOLINT(readability-non-const-parameter)
static int8_t cdc_transmit_cplt_fs(uint8_t *buf, uint32_t *len, uint8_t epnum);  // NOLINT(readability-non-const-parameter)

/* Empfangs-Hook aus byte_io.c (Transportschicht): cdc_collect legt die Bytes
   in den Ringpuffer; usb_rx_arm_when_space() enthaelt die EINZIGE Arm-Entscheidung
   des Projekts ( Deklarationen in cdc_input_if.h ). */

/* Empfangspuffer fuer den USB-Endpoint.
   Ohne SetRxBuffer in cdc_init_fs war RxBuffer NULL und cdc_collect
   las statt der Terminal-Daten den Flash ab Adresse 0 (Vektortabelle,
   erstes Byte = 0xE8 = 232). */
static uint8_t user_rx_buffer_fs[CDC_DATA_FS_OUT_PACKET_SIZE];

static int8_t cdc_init_fs(void) {
   /* Rx-Puffer im Klassen-Handle setzen */
   USBD_CDC_SetRxBuffer(&usbd_fs, &user_rx_buffer_fs[0]);
   return USBD_OK;
}

static int8_t cdc_deinit_fs(void) { return USBD_OK; }

static int8_t cdc_control_fs(uint8_t cmd, uint8_t *pbuf, uint16_t len) {  // NOLINT(readability-non-const-parameter)
   UNUSED(cmd);
   UNUSED(pbuf);
   UNUSED(len);
   return USBD_OK; /* Line-Coding etc. ignorieren (nur informativ bei CDC) */
}

static int8_t cdc_receive_fs(uint8_t *buf, uint32_t *len) {  // NOLINT(readability-non-const-parameter)
   /* RACE-FIX: Das Endpoint wird NUR NOCH ueber usb_rx_arm_when_space()
      aufgeruestet -- dort laeuft Pruefen + Setzen von rx_armed inkl.
      USBD_CDC_ReceivePacket() unter IRQ-Sperre. Main-Loop (byte_io_get) und
      ISR koennen damit dasselbe Endpoint nie doppelt aufruesten; genau
      dieses Doppel-Aufruesten hatte nach dem ersten Ring-Voll-Ereignis
      verschobene Bytes produziert.

      cdc_collect selbst ruft KEIN ReceivePacket mehr und entscheidet
      auch nichts mehr -- die Arm-Entscheidung liegt vollstaendig in
      usb_rx_arm_when_space(). */
   cdc_collect(buf, *len);  /* in den Sammelpuffer (Ring) */
   usb_rx_arm_when_space(); /* einzige Arm-Stelle, atomar */
   return USBD_OK;
}

/* Completion-Callback der Middleware (USBD_CDC_DataIn), nach jedem Tx-Transfer.
   Ohne diesen Eintrag stand im fops-Struct NULL -> HardFault beim Terminal-Oeffnen. */
static int8_t cdc_transmit_cplt_fs(uint8_t *buf, uint32_t *len, uint8_t epnum) {  // NOLINT(readability-non-const-parameter)
   UNUSED(buf);
   UNUSED(len);
   UNUSED(epnum);
   return USBD_OK;
}

/* Designated Initializers: Felder explizit benannt, Reihenfolge egal
   (positional waere eine Stolperfalle, siehe PROJECT.md Abschnitt 7) */
USBD_CDC_ItfTypeDef usbd_interface_fops_fs = {
   .Init = cdc_init_fs,
   .DeInit = cdc_deinit_fs,
   .Control = cdc_control_fs,
   .Receive = cdc_receive_fs,
   .TransmitCplt = cdc_transmit_cplt_fs,
};

/* static: nur der Wrapper cdc_transmit() (Schnittstelle cdc_input_if.h) ruft
   diese Funktion -- bewusst kein Eintrag in usbd_cdc_if.h */
static uint8_t cdc_transmit_fs(uint8_t *buf, uint16_t len) {
   USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)usbd_fs.pClassData;

   if (hcdc == NULL) {
      return USBD_FAIL; /* Klasse noch nicht initial. */
   }
   hcdc->TxBuffer = buf;
   hcdc->TxLength = len;

   if (usbd_fs.dev_state == USBD_STATE_CONFIGURED) {
      return USBD_CDC_TransmitPacket(&usbd_fs);
   }
   return USBD_FAIL;
}

/* ------------------------------------------------------------------ */
/* Wrapper fuer die Anwendung (implementiert hier, deklariert in      */
/* cdc_input_if.h): USB-Interna bleiben in dieser Datei               */
/* ------------------------------------------------------------------ */

bool cdc_rx_arm(void) {
   /* Reiner Hardware-Aufruf; die Arm-ENTSCHEIDUNG (rx_armed + Ring-Freiplatz
      unter IRQ-Sperre) liegt in usb_rx_arm_when_space() (byte_io.c). */
   return USBD_CDC_ReceivePacket(&usbd_fs) == USBD_OK;
}

bool cdc_tx_busy(void) {
   USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)usbd_fs.pClassData;

   return (hcdc != NULL) && (hcdc->TxState != 0U);
}

uint8_t cdc_transmit(uint8_t *buf, uint16_t len) { return cdc_transmit_fs(buf, len); }
