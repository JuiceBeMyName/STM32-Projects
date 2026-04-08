#include "stm32l47xxx_systick.h"

/*********************************************************************
 * @fn      		- delay_us
 *
 * @brief           - Provides a simple software blocking delay
 *
 * @param[in]		- us: Number of microseconds to delay
 *
 * @return			- None
 *
 * @note			- Uses a NOP loop. Leaves the SysTick hardware timer
 * running so global system time (gSystemMsCount)
 * is never interrupted or corrupted.
 *********************************************************************/
void delay_us(uint32_t us)
{
    // A standard C for-loop with a NOP and a volatile counter
    // takes at minimum 4-5 clock cycles per iteration.
    // Ticks per microsecond = SYSTEM_CLOCK_MHZ.

    uint32_t iterations = us * (SYSTEM_CLOCK_MHZ / 4);

    for (volatile uint32_t i = 0; i < iterations; i++) {
        __asm __vo ("nop");
    }
}
