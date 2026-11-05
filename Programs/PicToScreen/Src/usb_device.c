/**
 ******************************************************************************
 * @file    usb_device.c
 * @brief   Startpunkt des USB-Device-Stacks (Implementierung von
 *          usb_device.h): USB-Stack einmalig hochfahren (USBD_Init, CDC-Klasse
 *          registrieren, USBD_Start). Besitzt das Geraete-Handle usbd_fs.
 *          Wird im Projekt ausschliesslich von byte_io_init() (Transportschicht,
 *          byte_io.c) aufgerufen.
 ******************************************************************************
 */
#include "usb_device.h"
#include "error.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"

USBD_HandleTypeDef usbd_fs;

void usb_device_init(void) {
   if (USBD_Init(&usbd_fs, &fs_desc, DEVICE_FS) != USBD_OK) {
      Error_Handler();
   }
   if (USBD_RegisterClass(&usbd_fs, &USBD_CDC) != USBD_OK) {
      Error_Handler();
   }
   if (USBD_CDC_RegisterInterface(&usbd_fs, &usbd_interface_fops_fs) != USBD_OK) {
      Error_Handler();
   }
   if (USBD_Start(&usbd_fs) != USBD_OK) {
      Error_Handler();
   }
}
