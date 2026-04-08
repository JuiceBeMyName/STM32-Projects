#include "stm32l47xxx_rcc_driver.h"

// These arrays help us quickly translate the bits in the register to actual division math
uint16_t AHB_PreScaler[16] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 64, 128, 256, 512};
uint8_t APB1_PreScaler[8]  = {1, 1, 1, 1, 2, 4, 8, 16};
uint8_t APB2_PreScaler[8]  = {1, 1, 1, 1, 2, 4, 8, 16};



/*********************************************************************
 * @fn      		- RCC_GetSysClk
 *
 * @brief           - Reads the RCC configuration registers to determine the
 * current System Clock (SYSCLK) source and frequency.
 *
 * @return			- uint32_t: The system clock frequency in Hz
 *
 * @note			- On STM32L4, MSI defaults to 4MHz. HSI is fixed at 16MHz.
 * Warning: HSE depends on the physical crystal on your board (typically 8MHz).
 * Warning: If using PLL, you must update this function with the full PLL math.
 *********************************************************************/
uint32_t RCC_GetSysClk(void)
{
	uint32_t sysclk = 0;
	uint8_t clksrc;

	// Read bits 3:2 (SWS) of the RCC_CFGR register to find the clock source
	clksrc = (RCC->CFGR >> 2) & 0x03;

	if (clksrc == 0) // MSI (Default out of reset for STM32L4)
	{
		sysclk = 4000000;	// Assuming default 4MHz MSI for now
	}
	else if (clksrc == 1) //HSI
	{
		sysclk = 16000000; // 16 MHz
	}
	else if (clksrc == 2) // HSE
	{
		sysclk = 8000000; // Typical external crystal speed
	}
	else if (clksrc == 3) // PLL
	{
		sysclk = 80000000; // Assuming PLL is configured for max 80MHz
	}

	return sysclk;
}



/*********************************************************************
 * @fn      		- RCC_GetPCLK1Value
 *
 * @brief           - Calculates the clock frequency of the APB1 Peripheral Bus
 * (Used by USART2, USART3, UART4, UART5, I2C1, I2C2, I2C3)
 *
 * @return			- uint32_t: The APB1 clock frequency in Hertz (Hz)
 *********************************************************************/
uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t sysclk, pclk1;
	uint8_t ahbp, apb1p;

	sysclk = RCC_GetSysClk();

	uint8_t temp = (RCC->CFGR >> 4) & 0xF;
	ahbp = AHB_PreScaler[temp];

	temp = (RCC->CFGR >> 8) & 0x7;
	apb1p = APB1_PreScaler[temp];

	pclk1 = (sysclk / ahbp) / apb1p;

	return pclk1;
}



/*********************************************************************
 * @fn      		- RCC_GetPCLK2Value
 *
 * @brief           - Calculates the clock frequency of the APB2 Peripheral Bus
 * (Used by USART1, SPI1)
 *
 * @return			- uint32_t: The APB2 clock frequency in Hertz (Hz)
 *********************************************************************/
uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t sysclk, pclk2;
	uint8_t ahbp, apb2p;

	sysclk = RCC_GetSysClk();

	uint8_t temp = (RCC->CFGR >> 4) & 0xF;
	ahbp = AHB_PreScaler[temp];

	temp = (RCC->CFGR >> 11) & 0x7;
	apb2p = APB2_PreScaler[temp];

	pclk2 = (sysclk / ahbp) / apb2p;

	return pclk2;
}



/*********************************************************************
 * @fn      		- RCC_SystemClock_ConfigTo16MHz
 *
 * @brief           - Wakes up the HSI oscillator and sets it as the main system clock
 *
 * @param[in]		- None
 *
 * @return			- None
 *
 * @note			- Bypasses the default 4MHz MSI clock.
 * Forces the CPU to run at 16 MHz.
 *********************************************************************/
void RCC_SystemClock_ConfigTo16MHz(void)
{
	// 1. Turn ON the HSI (High Speed Internal) 16 MHz Clock
	RCC->CR |= (1 << 8);

	// 2. Wait until the HSI is stable and ready
	while(!(RCC->CR & (1 << 10)));

	// 3. Switch the main System Clock to use the HSI
	RCC->CFGR &= ~(0x3);       // Clear the clock switch bits
	RCC->CFGR |= 0x1;          // Set it to HSI

	// 4. Wait for the hardware to confirm the switch is complete
	while(((RCC->CFGR >> 2) & 0x3) != 1);
}

