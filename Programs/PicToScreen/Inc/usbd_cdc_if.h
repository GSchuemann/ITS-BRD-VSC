/**
 ******************************************************************************
 * @file    usbd_cdc_if.h
 * @brief   CDC-Anwendungsadapter: Callback-Tabelle der USB-CDC-Klasse, die in
 *          usb_device_init() (usb_device.c) an USBD_CDC_RegisterInterface()
 *          uebergeben wird.
 ******************************************************************************
 */

#ifndef USBD_CDC_IF_H
#define USBD_CDC_IF_H

#include "usbd_cdc.h"

/**
 * @brief  Callback-Tabelle der CDC-Klasse (Init/DeInit/Control/Receive/
 *         TransmitCplt). Wird in usb_device_init() an USBD_CDC_RegisterInterface()
 *         uebergeben; Definition in usbd_cdc_if.c.
 */
extern USBD_CDC_ItfTypeDef usbd_interface_fops_fs;

#endif
