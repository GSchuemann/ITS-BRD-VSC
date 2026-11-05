/**
 ******************************************************************************
 * @file    usb_device.h
 * @brief   Startpunkt des USB-Device-Stacks: Geraete-Handle usbd_fs und
 *          Initialisierung des Stacks (USBD_Init, CDC-Klasse registrieren,
 *          USBD_Start).
 *
 *          Wird im Projekt ausschliesslich von byte_io_init() (Transportschicht,
 *          byte_io.c) aufgerufen.
 ******************************************************************************
 */

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "usbd_def.h"

/* Fallback, falls die verwendete USBD-Library-Version DEVICE_FS nicht
 * in usbd_def.h definiert (siehe USBD_Init-Signatur: device_class-Parameter) */
#ifndef DEVICE_FS
#define DEVICE_FS 0U /* Full-Speed-Device (im Ggs. zu DEVICE_HS = 1) */
#endif

/** USB-Device-Handle (Full-Speed); wird von usb_device_init() initialisiert. */
extern USBD_HandleTypeDef usbd_fs;

/**
 * @brief  Fahrt den USB-Device-Stack einmalig hoch: USBD_Init (Handle usbd_fs,
 *         Deskriptoren fs_desc, Full-Speed), CDC-Klasse registrieren
 *         (USBD_CDC mit Interface usbd_interface_fops_fs), USBD_Start.
 *         Bei Fehler jeweils Error_Handler().
 * @param  None
 * @retval None
 */
void usb_device_init(void);

#endif
