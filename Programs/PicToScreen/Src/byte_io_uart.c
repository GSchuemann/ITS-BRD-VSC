/**
 ******************************************************************************
 * @file    byte_io_uart.c
 * @brief   UART-via-USB Backend der Transportschicht (Interface siehe
 *          byte_io_backend.h, Dispatcher byte_io.c).
 *
 *          Transport fuer "UART via USB": Die STM32 spricht USART3 (115200 8N1,
 *          konfiguriert von initITSboard()/stdinout_USART.c der ITS_BRD_LIB --
 *          init_input() wird in main() nach initITSboard() aufgerufen), der PC
 *          haengt per USB-UART-Adapter (USB-CDC auf PC-Seite) daran.
 *
 *          Empfang: DMA1 Stream1 Channel4, circular mode, schreibt direkt in
 *          einen Ringpuffer. Der Fuellstand ergibt sich aus dem DMA-Data-Length-
 *          Register (geschrieben wird an Position SIZE_OF_RING_BUFFER -
 *          GetDataLength). Senden: polling TXE/TC.
 *
 *          Portiert aus dem frueheren input.c (usb_uart_read / usbUartWrite /
 *          usbUartDMAInt) auf das Backend-Interface byte_io_backend.h.
 *
 *          PROTOKOLL-Konsequenz (liegt in input.c): UART hat keine
 *          HW-Flusskontrolle, daher Burst-Request per 'D' nach jedem
 *          Laengenheader ('S' statt 'F').
 ******************************************************************************
 */

#include "byte_io_backend.h"
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_dma.h>
#include <stm32f4xx_ll_usart.h>
#include "core_cm4.h" /* NVIC_DisableIRQ, SystemCoreClock */
#include <stddef.h>
#include <stdint.h>

#define UART_BAUDRATE 115200                 /* 8N1, wie vom PC-Tool erwartet */
#define UART_BUF_SIZE 512                    /* Burst-Groesse wie im Protokoll (input.c) */
#define SIZE_OF_RING_BUFFER (3 * UART_BUF_SIZE)
#define PPRE1_MASK 0x7U /* 3-Bit-Feld PPRE1 in RCC->CFGR */

static char ring_buf[SIZE_OF_RING_BUFFER]; /* RX-Ringpuffer, DMA1 schreibt direkt hinein */
static int nxt_read_pos = 0;               /* naechste Leseposition im Ring */

/**
 * @brief  Liest bis zu len Bytes aus dem DMA-Ringpuffer (nicht blockierend).
 * @retval Anzahl gelesener Bytes (0, wenn der DMA-Puffer nichts Neues hat)
 */
static int uart_read(char *buf, size_t len) {
   // ToDo Check overflow of ring buffer
   for (size_t i = 0; i < len; i++) {
      int pos = SIZE_OF_RING_BUFFER - (int)LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_1);
      if (nxt_read_pos == pos) {
         return (int)i;
      }
      buf[i] = ring_buf[nxt_read_pos];
      nxt_read_pos = (nxt_read_pos + 1) % SIZE_OF_RING_BUFFER;
   }
   return (int)len;
}

/**
 * @brief  Sendet len Bytes ueber USART3 (blockierend, polling TXE/TC).
 */
static void uart_write(const char *buf, size_t len) {
   for (size_t i = 0; i < len; i++) {
      LL_USART_TransmitData8(USART3, buf[i]);
      while (!LL_USART_IsActiveFlag_TXE(USART3)) {
      }
   }
   while (!LL_USART_IsActiveFlag_TC(USART3)) {
   }
}

/**
 * @brief  Konfiguriert USART3-RX per DMA1 Stream1 Channel4 (circular) auf
 *         den Ringpuffer. USART3 selbst (GPIO/Takt/Baudrate) ist bereits
 *         von initITSboard() konfiguriert.
 */
static void uart_dma_init(void) {
   /* Der CMSIS-Driver (stdinout_USART.c via initITSboard) hat RXNE/IDLE/EIE
      und den USART3-NVIC-IRQ eingeschaltet; sein IRQ-Handler liest DR ohne
      gearmten Receive (Dummy-Read) und verwirft dadurch jedes empfangene
      Byte. Fuer reinen DMA-Empfang hier zuerst abschalten, sonst landet
      nichts im Ringpuffer und der Handshake ('H' -> 'R') bleibt aus. */
   LL_USART_DisableIT_RXNE(USART3);
   LL_USART_DisableIT_IDLE(USART3);
   LL_USART_DisableIT_ERROR(USART3);
   NVIC_DisableIRQ(USART3_IRQn);

   /* Baudrate neu setzen: initITSboard() hat den BRR bei 180 MHz (APB1 = 45 MHz)
      berechnet, switch_to_168_MHz() aendert APB1 danach auf 42 MHz. Ohne
      Nachziehen laeuft USART3 ~7 % zu langsam und jedes Byte wird verfaelscht
      (aus 'H' = 0x48 wird z.B. 0xA8). */
   LL_USART_Disable(USART3);
   /* PCLK1 aus den Registern bestimmen (LL_RCC_GetSystemClocksFreq ist ohne
      USE_FULL_LL_DRIVER nicht verfuegbar): AHB = SYSCLK (/1), APB1-Prascaler
      aus RCC->CFGR. SystemCoreClock pflegt HAL_RCC_ClockConfig nach. */
   static const uint8_t APB_PRESC[8] = {0, 0, 0, 0, 1, 2, 3, 4}; /* PPRE: /1 /2 /4 /8 /16 */
   uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
   uint32_t pclk1 = SystemCoreClock >> APB_PRESC[ppre1 & PPRE1_MASK];
   LL_USART_SetBaudRate(USART3, pclk1, LL_USART_OVERSAMPLING_16, UART_BAUDRATE);
   LL_USART_Enable(USART3);

   /* Peripheral clock enable */
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
   /* USART3 DMA Init */
   LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_1, LL_DMA_CHANNEL_4);
   LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_1,
                                   LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
   LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_1, LL_DMA_PRIORITY_LOW);
   LL_DMA_SetMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MODE_CIRCULAR);
   LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_PERIPH_NOINCREMENT);
   LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MEMORY_INCREMENT);
   LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_1, LL_DMA_PDATAALIGN_BYTE);
   LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_1, LL_DMA_MDATAALIGN_BYTE);
   LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_1);
   LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1,
                           LL_USART_DMA_GetRegAddr(USART3));
   LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)ring_buf);
   LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, SIZE_OF_RING_BUFFER);
   LL_USART_EnableDMAReq_RX(USART3);
   /* Enable USART and DMA */
   LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
}

/* ========================================================================== */
/* Backend-Interface (byte_io_backend.h), gereicht ueber byte_io.c           */
/* ========================================================================== */

void byte_io_uart_init(void) {
   nxt_read_pos = 0;
   uart_dma_init();
}

uint8_t byte_io_uart_get(void) {
   char chr;
   while (0 == uart_read(&chr, 1)) { /* blockierend warten, bis ein Byte anliegt */
   }
   return (uint8_t)chr;
}

void byte_io_uart_put(uint8_t byte) {
   char chr = (char)byte;
   uart_write(&chr, 1);
}

void byte_io_uart_clear(void) {
   char chr;
   while (0 != uart_read(&chr, 1)) { /* Ringpuffer leeren */
   }
}

// EOF
