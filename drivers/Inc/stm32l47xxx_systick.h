#ifndef STM32L47XXX_SYSTICK_H
#define STM32L47XXX_SYSTICK_H

#include "stm32l47xxx.h"

/*
 * Blocking microsecond delay
 */
void delay_us(uint32_t us);

// New Non-Blocking Interrupt Setup Function
void SysTick_Init(uint32_t tick_count);

#endif /* STM32L47XXX_SYSTICK_H */
