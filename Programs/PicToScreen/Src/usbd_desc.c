/*
 * ============================================================================
 *  usbd_desc.c  --  USB-Identitaet des Geraets (Deskriptor-Tabellen)
 * ============================================================================
 *  Aufgabe: Device-, Konfigurations- und String-Deskriptoren als reine
 *  Datentabellen bereitstellen (USBD_Descriptors-Struct fs_desc): VID/PID,
 *  Klassen-Codes, Strings. Windows bindet Treiber primaer per VID/PID
 *  (CDC-PID 0x5740 statt DFU-PID -- Details: PROJECT.md, Abschnitt 3).
 */

/**
 ******************************************************************************
 * @file    USB_Device/CDC_Standalone/Src/usbd_desc.c
 * @author  MCD Application Team
 * @brief   This file provides the USBD descriptors and string formatting method.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright � 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "usbd_desc.h"
#include "usbd_conf.h"
#include "usbd_def.h"           /* USBD_SpeedTypeDef, USBD_DescriptorsTypeDef, LOBYTE/HIBYTE, USB_DESC_*, __ALIGN_* */
#include "usbd_ctlreq.h"        /* USBD_GetString */
#include "stm32f4xx_hal_def.h"  /* UNUSED (hal.h ist ueber usbd_conf.h schon geladen, daher kein Zyklus) */
#include <stdint.h>             /* uint8_t, uint16_t, uint32_t */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USBD_VID 0x0483
/* ST "Virtual COM Port"-PID. NICHT 0xDF11 (DFU-Bootloader): VID/PID 0483:DF11
   laesst Windows den ST-DFuSe-Treiber-Match greifen -> Geraet erscheint als
   "STM DfuSe in FS Mode" ohne Treiber statt als COM-Port. */
#define USBD_PID 0x5740
#define USBD_LANGID_STRING 0x409
#define USBD_MANUFACTURER_STRING "STMicroelectronics"
#define USBD_PRODUCT_HS_STRING "STM32 Virtual ComPort in HS Mode"
#define USBD_PRODUCT_FS_STRING "STM32 Virtual ComPort in FS Mode"
#define USBD_CONFIGURATION_FS_STRING "CDC Config"
#define USBD_INTERFACE_FS_STRING "CDC Interface"

/* Adressen des Unique-Device-ID-Registers (96 Bit, RM0090); der Inhalt ist
   pro Chip verschieden und dient als USB-Seriennummer */
#define DEVICE_ID1 (0x1FFF7A10)
#define DEVICE_ID2 (0x1FFF7A14)
#define DEVICE_ID3 (0x1FFF7A18)

/* Laenge des Seriennummer-String-Descriptors in Bytes (bLength + Type + 12 hex chars) */
#define USB_SIZ_STRING_SERIAL 0x1A

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint8_t *usbd_cdc_device_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *usbd_cdc_lang_id_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *usbd_cdc_manufacturer_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *usbd_cdc_product_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *usbd_cdc_serial_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *usbd_cdc_config_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *usbd_cdc_interface_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length);

/* Private variables ---------------------------------------------------------*/
USBD_DescriptorsTypeDef fs_desc = {
    usbd_cdc_device_descriptor,       usbd_cdc_lang_id_str_descriptor, usbd_cdc_manufacturer_str_descriptor,
    usbd_cdc_product_str_descriptor,   usbd_cdc_serial_str_descriptor, usbd_cdc_config_str_descriptor,
    usbd_cdc_interface_str_descriptor,
};

/* USB Standard Device Descriptor */
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN static uint8_t usbd_device_desc[USB_LEN_DEV_DESC] __ALIGN_END = {
    0x12,                 /* bLength */ // NOLINT(readability-magic-numbers)
    USB_DESC_TYPE_DEVICE, /* bDescriptorType */
    0x00,                 /* bcdUSB */
    0x02,
    0x02,             /* bDeviceClass: CDC (0x00 wuerde Windows als
                          Composite werten; mit 0x02 bindet usbser direkt) */
    0x00,             /* bDeviceSubClass */
    0x00,             /* bDeviceProtocol */
    USB_MAX_EP0_SIZE, /* bMaxPacketSize */
    LOBYTE(USBD_VID), /* idVendor */
    HIBYTE(USBD_VID), /* idVendor */
    LOBYTE(USBD_PID), /* idVendor */
    HIBYTE(USBD_PID), /* idVendor */
    0x00,             /* bcdDevice rel. 2.00 */
    0x02,
    USBD_IDX_MFC_STR,          /* Index of manufacturer string */
    USBD_IDX_PRODUCT_STR,      /* Index of product string */
    USBD_IDX_SERIAL_STR,       /* Index of serial number string */
    USBD_MAX_NUM_CONFIGURATION /* bNumConfigurations */
}; /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN static uint8_t usbd_lang_id_desc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN static uint8_t usbd_string_serial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
    USB_SIZ_STRING_SERIAL,
    USB_DESC_TYPE_STRING,
};

#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN static uint8_t usbd_str_desc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

/* Private functions ---------------------------------------------------------*/
static void int_to_unicode(uint32_t value, uint8_t *pbuf, uint8_t len);
static void get_serial_num(void);

/**
 * @brief  Returns the device descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_device_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   /* Prevent unused argument(s) compilation warning */
   UNUSED(speed);

   *length = sizeof(usbd_device_desc);
   return (uint8_t *)usbd_device_desc;
}

/**
 * @brief  Returns the LangID string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_lang_id_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   /* Prevent unused argument(s) compilation warning */
   UNUSED(speed);

   *length = sizeof(usbd_lang_id_desc);
   return (uint8_t *)usbd_lang_id_desc;
}

/**
 * @brief  Returns the product string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_product_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   if (speed == USBD_SPEED_HIGH) {
      USBD_GetString((uint8_t *)USBD_PRODUCT_HS_STRING, usbd_str_desc, length);
   } else {
      USBD_GetString((uint8_t *)USBD_PRODUCT_FS_STRING, usbd_str_desc, length);
   }
   return usbd_str_desc;
}

/**
 * @brief  Returns the manufacturer string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_manufacturer_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   /* Prevent unused argument(s) compilation warning */
   UNUSED(speed);

   USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, usbd_str_desc, length);
   return usbd_str_desc;
}

/**
 * @brief  Returns the serial number string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_serial_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   /* Prevent unused argument(s) compilation warning */
   UNUSED(speed);

   *length = USB_SIZ_STRING_SERIAL;

   /* Update the serial number string descriptor with the data from the unique ID*/
   get_serial_num();

   return (uint8_t *)usbd_string_serial;
}

/**
 * @brief  Returns the configuration string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_config_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   /* Nur OTG_FS (Full Speed) wird genutzt -- es gibt keinen HS-String */
   UNUSED(speed);

   USBD_GetString((uint8_t *)USBD_CONFIGURATION_FS_STRING, usbd_str_desc, length);
   return usbd_str_desc;
}

/**
 * @brief  Returns the interface string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
static uint8_t *usbd_cdc_interface_str_descriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
   /* Nur OTG_FS (Full Speed) wird genutzt -- es gibt keinen HS-String */
   UNUSED(speed);

   USBD_GetString((uint8_t *)USBD_INTERFACE_FS_STRING, usbd_str_desc, length);
   return usbd_str_desc;
}

/**
 * @brief  Create the serial number string descriptor
 * @param  None
 * @retval None
 */
static void get_serial_num(void) {
   uint32_t deviceserial0;
   uint32_t deviceserial1;
   uint32_t deviceserial2;

   deviceserial0 = *(uint32_t *)DEVICE_ID1;
   deviceserial1 = *(uint32_t *)DEVICE_ID2;
   deviceserial2 = *(uint32_t *)DEVICE_ID3;

   deviceserial0 += deviceserial2;

   if (deviceserial0 != 0) {
      int_to_unicode(deviceserial0, &usbd_string_serial[2], 8);   // NOLINT(readability-magic-numbers)
      int_to_unicode(deviceserial1, &usbd_string_serial[18], 4);  // NOLINT(readability-magic-numbers)
   }
}

/**
 * @brief  Convert Hex 32Bits value into char
 * @param  value: value to convert
 * @param  pbuf: pointer to the buffer
 * @param  len: buffer length
 * @retval None
 */
static void int_to_unicode(uint32_t value, uint8_t *pbuf, uint8_t len) {
   uint8_t idx = 0;

   for (idx = 0; idx < len; idx++) {
      if (((value >> 28)) < 0xA) {  // NOLINT(readability-magic-numbers)
         pbuf[2 * idx] = (value >> 28) + '0';  // NOLINT(readability-magic-numbers)
      } else {
         pbuf[2 * idx] = (value >> 28) + 'A' - 10;  // NOLINT(readability-magic-numbers)
      }

      value = value << 4;

      pbuf[(2 * idx) + 1] = 0;
   }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
