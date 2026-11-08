#ifndef TEST_STM32F4XX_H
#define TEST_STM32F4XX_H
#include <stdint.h>

typedef struct {
   volatile uint32_t CNT;
} TIM_TypeDef;

extern TIM_TypeDef test_tim2;
#define TIM2 (&test_tim2)

#endif