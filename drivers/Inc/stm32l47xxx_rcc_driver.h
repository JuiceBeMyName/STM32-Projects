#ifndef STM32L47XXX_RCC_DRIVER_H_
#define STM32L47XXX_RCC_DRIVER_H_

#include "stm32l47xxx.h"

// Function Prototypes
uint32_t RCC_GetPCLK1Value(void);
uint32_t RCC_GetPCLK2Value(void);

void RCC_SystemClock_ConfigTo16MHz(void);

#endif /* STM32L47XXX_RCC_DRIVER_H_ */
