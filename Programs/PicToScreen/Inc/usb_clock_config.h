/**
 ******************************************************************************
 * @file    usb_clock_config.h
 * @brief   Umschalten der CPU-Clock zur Erzeugung des exakten 48-MHz-Takts
 *          fuer den OTG_FS-Controller (USB).
 ******************************************************************************
 */

#ifndef USB_CLOCK_CONFIG_H
#define USB_CLOCK_CONFIG_H

/**
 * @brief  Stellt die CPU- und Bustakte auf die "USB-sichere" Konfiguration um:
 *         SysClock = 168 MHz (statt 180 MHz).
 */
extern void switch_to_168_MHz(void);  // NOLINT(readability-identifier-naming)

#endif
