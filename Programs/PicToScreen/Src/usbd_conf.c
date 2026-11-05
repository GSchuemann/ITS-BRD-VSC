/*
 * ============================================================================
 *  usbd_conf.c  --  Portierungsschicht USB-Middleware <-> HAL/OTG-Core
 * ============================================================================
 *  Aufgabe: Alle HAL_PCD_*-Callbacks an USBD_LL_* weiterreichen, GPIO-/Takt-/
 *  NVIC-Konfiguration (MspInit) und den ISR-Vektor OTG_FS_IRQHandler
 *  bereitstellen. Besitzt das PCD-Handle hpcd.
 */

/**
 ******************************************************************************
 * @file    usbd_conf.c
 * @brief   USB Device library callbacks and MSP (CDC-Variante, NUCLEO-F429ZI)
 *
 *  Basis: USB_Device/DFU_Standalone/Src/usbd_conf.c aus dem STM32CubeF4
 *  (STM32F429ZI-Nucleo). Fuer die CDC-Klasse vorgenommene Aenderungen:
 *
 *    1. FIFO-Konfiguration erweitert: CDC benoetigt zusaetzlich TX-FIFOs
 *       fuer EP1-IN (Bulk-Daten) und EP2-IN (Command/Notification).
 *       Nur EP0-FIFO (DFU-Original) fuehrt zu stillem Sendefehler.
 *    2. VBUS-Sensing deaktiviert: Auf dem NUCLEO-F429ZI liegt an PA9
 *       nicht immer ein sauberer VBUS-Pegel an (Known Issue); ohne
 *       Sensing verhaelt sich das Device wie bus-powered -> Enumeration.
 *       Der PA9-GPIO-Block entfaellt dadurch.
 *    3. Includes ergaenzt (usbd_def.h, usbd_core.h, usbd_conf.h), damit
 *       die Datei eigenstaendig kompiliert, auch wenn main.h die
 *       USB-Header nicht mehr zieht.
 *
 *  Handle heisst "hpcd" (Original des Beispiels). stm32f4xx_it.c muss
 *  OTG_FS_IRQHandler() mit HAL_PCD_IRQHandler(&hpcd) bereitstellen.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution of this software other than as permitted under this
 *    license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED
 * TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

 // NOLINTBEGIN

/* Includes ------------------------------------------------------------------*/
#include "usbd_conf.h" /* Projekt-Header (haelt MAX_STATIC_ALLOC_SIZE) + HAL  */
#include "usbd_core.h" /* USBD_LL_* Prototypen                           */
#include "usbd_def.h"  /* USBD_Types, USBD_HandleTypeDef (fuer LL-If)   */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PCD_HandleTypeDef hpcd;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
                       PCD BSP Routines
*******************************************************************************/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
   GPIO_InitTypeDef GPIO_InitStruct;

   /* Configure USB FS GPIOs */
   __HAL_RCC_GPIOA_CLK_ENABLE();

   /* Configure DM DP Pins */
   GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Hinweis: VBUS-Pin PA9 entfaellt -- vbus_sensing_enable = 0
    * (siehe Aenderungshistorie im Dateikopf).                          */

   /* Configure ID pin */
   GPIO_InitStruct.Pin = GPIO_PIN_10;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Enable USB FS Clock */
   __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

   /* Set USBFS Interrupt priority */
   HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);

   /* Enable USBFS Interrupt */
   HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

/**
 * @brief  De-Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
   /* Disable USB FS Clock */
   __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
}

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/

/**
 * @brief  SetupStage callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup); }

/**
 * @brief  DataOut Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
   USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
 * @brief  DataIn Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
   USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
 * @brief  SOF callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_SOF(hpcd->pData); }

/**
 * @brief  Reset callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
   USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

   /* Set USB Current Speed */
   switch (hpcd->Init.speed) {
   case PCD_SPEED_HIGH:
      speed = USBD_SPEED_HIGH;
      break;

   case PCD_SPEED_FULL:
      speed = USBD_SPEED_FULL;
      break;

   default:
      speed = USBD_SPEED_FULL;
      break;
   }

   /* Reset Device */
   USBD_LL_Reset(hpcd->pData);

   USBD_LL_SetSpeed(hpcd->pData, speed);
}

/**
 * @brief  Suspend callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_Suspend(hpcd->pData); }

/**
 * @brief  Resume callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_Resume(hpcd->pData); }

/**
 * @brief  ISOOUTIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
   USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ISOINIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
   USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ConnectCallback callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_DevConnected(hpcd->pData); }

/**
 * @brief  Disconnect callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_DevDisconnected(hpcd->pData); }

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
 * @brief  Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev) {
   /* Set LL Driver parameters */
   hpcd.Instance = USB_OTG_FS;
   hpcd.Init.dev_endpoints = 4;
   hpcd.Init.use_dedicated_ep1 = 0;
   hpcd.Init.dma_enable = 0;
   hpcd.Init.low_power_enable = 0;
   hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
   hpcd.Init.Sof_enable = 0;
   hpcd.Init.speed = PCD_SPEED_FULL;
   /* AENDERUNG ggueber DFU-Original: VBUS-Sensing aus (NUCLEO-F429ZI,
    * PA9 liefert oft keinen sauberen VBUS-Pegel -> keine Enumeration).  */
   hpcd.Init.vbus_sensing_enable = 0;
   hpcd.Init.lpm_enable = 0;

   /* Link The driver to the stack */
   hpcd.pData = pdev;
   pdev->pData = &hpcd;

   /* Initialize LL Driver */
   HAL_PCD_Init(&hpcd);

   /* AENDERUNG ggueber DFU-Original: FIFO-Setup fuer CDC.
    *
    * CDC benoetigt (siehe usbd_cdc.h, typischerweise):
    *   EP0 (0x80)  Control, 64 Byte
    *   EP1 (0x81)  Bulk-IN  (Daten STM32 -> PC)
    *   EP1 (0x01)  Bulk-OUT (Daten PC -> STM32, laeuft ueber RxFIFO)
    *   EP2 (0x82)  Interrupt-IN (CDC Command/Notification)
    *
    * TX-FIFOs sind pro IN-Endpoint noetig; Bulk-OUT teilt sich die
    * RxFIFO.  Werte in 32-Bit-Woertern.  Das OTG_FS des F429 hat
    * 320 Woerter (1,25 KB) FIFO-RAM; diese Aufteilung belegt 192
    * Woerter und laesst Reserve fuer Setup-Pakete und Statuspakete:
    */
   HAL_PCDEx_SetRxFiFo(&hpcd, 0x50);    /* RX :  80 Woerter = 320 Byte        */
   HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x20); /* EP0:  32 Woerter = 128 Byte (Ctrl) */
   HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x40); /* EP1:  64 Woerter = 256 Byte (Bulk-IN) */
   HAL_PCDEx_SetTxFiFo(&hpcd, 2, 0x10); /* EP2:  16 Woerter =  64 Byte (Notif.)  */

   /* Summe: 0x50 + 0x20 + 0x40 + 0x10 = 0xC0 = 192 Woerter < 320       */

   return USBD_OK;
}

/**
 * @brief  De-Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) {
   HAL_PCD_DeInit(pdev->pData);
   return USBD_OK;
}

/**
 * @brief  Starts the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev) {
   HAL_PCD_Start(pdev->pData);
   return USBD_OK;
}

/**
 * @brief  Stops the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) {
   HAL_PCD_Stop(pdev->pData);
   return USBD_OK;
}

/**
 * @brief  Opens an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  ep_type: Endpoint Type
 * @param  ep_mps: Max Packet Size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
   HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

   return USBD_OK;
}

/**
 * @brief  Closes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval None
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
   HAL_PCD_EP_Close(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
   HAL_PCD_EP_Flush(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
   HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
   HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Stall (1: Yes, 0: No)
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
   PCD_HandleTypeDef *hpcd = pdev->pData;

   if ((ep_addr & 0x80) == 0x80) {
      return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
   } else {
      return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
   }
}

/**
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  dev_addr: USB address
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr) {
   HAL_PCD_SetAddress(pdev->pData, dev_addr);
   return USBD_OK;
}

/**
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size) {
   HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
   return USBD_OK;
}

/**
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size) {
   HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
   return USBD_OK;
}

/**
 * @brief  Returns the last received packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Received Data Size
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
   return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/**
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 */
void USBD_LL_Delay(uint32_t Delay) { HAL_Delay(Delay); }

/**
 * @brief  static single allocation.
 * @param  size: size of allocated memory
 * @retval None
 */
void *USBD_static_malloc(uint32_t size) {
   static uint32_t mem[MAX_STATIC_ALLOC_SIZE];
   return mem;
}

/**
 * @brief  Dummy memory free
 * @param  *p pointer to allocated memory address
 * @retval None
 */
void USBD_static_free(void *p) { (void)p; }

/* ---- USB ISR: Vektortabellen-Eintrag OTG_FS_IRQHandler -------------
 * Bewusst NICHT in der geteilten stm32f4xx_it.c (wird von mehreren
 * Projekten genutzt), sondern hier im USB-Modul: Das Startup-File
 * deklariert den Handler als [WEAK]; diese starke Definition verdraengt
 * die Weak-Referenz beim Linken automatisch. Projekte ohne das
 * USB-Feature bauen diese Datei nicht mit -> es bleibt beim Weak-
 * Default (Endlosschleife), der nie aufgerufen wird, weil der
 * OTG_FS-Interrupt dort nirgends enabled ist.
 * ------------------------------------------------------------------ */
void OTG_FS_IRQHandler(void) { HAL_PCD_IRQHandler(&hpcd); }

 // NOLINTEND


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
