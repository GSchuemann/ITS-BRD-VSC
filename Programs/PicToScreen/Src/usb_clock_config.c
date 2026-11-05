/**
 ******************************************************************************
 * @file    usb_clock_config.c
 * @brief   Implementierung von usb_clock_config.h: CPU-/Bustakte auf die
 *          "USB-sichere" Konfiguration umstellen (SysClock 168 MHz statt
 *          180 MHz).
 *
 *          Hintergrund: Der OTG_FS-Controller benoetigt einen exakten Kernel-
 *          Takt von 48 MHz. Die 48-MHz-Domaene (OTG_FS, SDIO, RNG) haengt beim
 *          F42x/43x fest an PLLQ der Haupt-PLL. Das ITS-Board laeuft
 *          normalerweise mit 180 MHz CPU-Takt (davon werden u. a. Timer und
 *          SPI-Clock abgeleitet); die Main-PLL liefert in dieser Konfiguration
 *          kein exaktes 48 MHz (Q=7 -> 45 MHz), und es gibt keine separate PLL
 *          fuer den OTG_FS-Kern. Daher wird die CPU auf 168 MHz reduziert, aus
 *          der die 48 MHz abgeleitet werden koennen.
 *
 *          Konsequenzen:
 *            - SPI-Bus etwas langsamer (kein Problem).
 *            - PWM aendert sich (bei uns nur Display-Helligkeit).
 *            - Zeitmessung mit TIM8 muss von der neuen Frequenz ausgehen.
 ******************************************************************************
 */

#include "usb_clock_config.h"
#include "error.h"
/* stm32f4xx_hal.h muss der erste HAL-Include bleiben: stm32f4xx_hal_def.h
   inkludiert stm32f4xx.h, das bei USE_HAL_DRIVER wieder stm32f4xx_hal.h
   laedt -- ein Modul-Header allein als erster Include fuehrt so zu einem
   Zyklus mit undefiniertem HAL_StatusTypeDef. */
#include "stm32f4xx_hal.h"  // IWYU pragma: keep
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "stm32f4xx_hal_rcc.h"

void switch_to_168_MHz(void) {
   RCC_OscInitTypeDef rcc_osc_init_struct = {0};
   RCC_ClkInitTypeDef rcc_clk_init_struct = {0};
   /* --- Stufe 1: SYSCLK von der PLL loesen und auf HSE schalten.
      Der Kern laeuft kurz mit 8 MHz – alles laeuft langsamer, aber stabil. */
   rcc_clk_init_struct.ClockType = RCC_CLOCKTYPE_SYSCLK;
   rcc_clk_init_struct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
   rcc_clk_init_struct.AHBCLKDivider = RCC_SYSCLK_DIV1; /* beim Rueckschalten wieder /1 */
   rcc_clk_init_struct.APB1CLKDivider = RCC_HCLK_DIV4;
   rcc_clk_init_struct.APB2CLKDivider = RCC_HCLK_DIV2;
   if (HAL_RCC_ClockConfig(&rcc_clk_init_struct, FLASH_LATENCY_5) != HAL_OK) {
      Error_Handler(); /* Latency 5 WS ist fuer 8 MHz uebersichert, unkritisch */
   }
   /* --- Stufe 2: PLL kann jetzt bedenkenlos aus- und neu konfiguriert werden */
   rcc_osc_init_struct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   rcc_osc_init_struct.HSEState = RCC_HSE_ON;
   rcc_osc_init_struct.PLL.PLLState = RCC_PLL_ON;
   rcc_osc_init_struct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   rcc_osc_init_struct.PLL.PLLM = 8; // NOLINT(readability-magic-numbers)
   rcc_osc_init_struct.PLL.PLLN = 336;           // NOLINT(readability-magic-numbers) 336 MHz VCO
   rcc_osc_init_struct.PLL.PLLP = RCC_PLLP_DIV2; /* 168 MHz     */
   rcc_osc_init_struct.PLL.PLLQ = 7;             // NOLINT(readability-magic-numbers) 48 MHz USB 
   if (HAL_RCC_OscConfig(&rcc_osc_init_struct) != HAL_OK) {
      Error_Handler();
   }
   /* --- Stufe 3: SYSCLK auf die neue PLL rueckschalten (168 MHz) */
   rcc_clk_init_struct.ClockType =
       (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   rcc_clk_init_struct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   rcc_clk_init_struct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   rcc_clk_init_struct.APB1CLKDivider = RCC_HCLK_DIV4; /* 42 MHz  */
   rcc_clk_init_struct.APB2CLKDivider = RCC_HCLK_DIV2; /* 84 MHz  */
   /* SystemCoreClock + SysTick werden hier automatisch nachgezogen, HAL_Delay() bleibt korrekt */
   if (HAL_RCC_ClockConfig(&rcc_clk_init_struct, FLASH_LATENCY_5) != HAL_OK) {
      Error_Handler();
   }
}

// EOF