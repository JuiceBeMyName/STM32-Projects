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



/*********************************************************************
 * @fn      		- SysTick_Init
 *
 * @brief           - Configures SysTick timer to generate background interrupts
 *
 * @param[in]		- tick_count: Number of clock cycles before an interrupt fires
 *
 * @return			- None
 *
 * @note			- Uses the core ARM Cortex-M registers
 *********************************************************************/
void SysTick_Init(uint32_t tick_count)
{
	// ARM Cortex-M SysTick Register Addresses
	uint32_t *pSCSR = (uint32_t*)0xE000E010; // SysTick Control and Status Register
	uint32_t *pSRVR = (uint32_t*)0xE000E014; // SysTick Reload Value Register
	uint32_t *pSCVR = (uint32_t*)0xE000E018; // SysTick Current Value Register

	// 1. Program the reload value
	// (We subtract 1 because the hardware counts down to 0)
	*pSRVR = tick_count - 1;

	// 2. Clear the current value register to 0 before starting
	*pSCVR = 0;

	// 3. Configure the Control Register
	// Bit 2 (CLKSOURCE) = 1 (Use processor clock)
	// Bit 1 (TICKINT)   = 1 (Enable the SysTick Exception/Interrupt!)
	// Bit 0 (ENABLE)    = 1 (Turn the timer on)
	*pSCSR |= (1 << 2) | (1 << 1) | (1 << 0);
}
