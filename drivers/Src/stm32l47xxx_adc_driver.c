#include "stm32l47xxx_adc_driver.h"

/*********************************************************************
 * @fn      		- ADC_PCLK_Control
 *
 * @brief           - Enables or Disables clock for ADC
 *
 * @param			- Base address of the ADC peripheral
 * @param			- ENABLE or DISABLE macros
 *
 * @return			- None
 *
 * @note			- Uses the RCC register to toggle peripheral clock
 *********************************************************************/
void ADC_PCLK_Control(ADC_RegDef_t *pADCx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		ADC_PCLK_EN();
	} else
	{
		ADC_PCLK_DI();
	}
}



/*********************************************************************
 * @fn      		- ADC_Init
 *
 * @brief           - Configures Power, Calibration, Resolution, and Sample Time
 *
 * @param[]			- Handle structure containing base address and config
 *
 * @return			- None
 *
 * @note			- This function handles the complex STM32L4 power-up sequence
 *
 *********************************************************************/
void ADC_Init(ADC_Handle_t *pADCHandle)
{
	// 1. Enable the peripheral clock
	ADC_PCLK_Control(pADCHandle->pADCx, ENABLE);

	// 2. Exit Deep Power Down mode (The ADC is physically disconnected from power by default)
	CLEAR_BIT(pADCHandle->pADCx->CR, ADC_CR_DEEPPWD_POS);

	// 3. Enable the ADC Voltage Regulator
	SET_BIT(pADCHandle->pADCx->CR, ADC_CR_ADVREGEN_POS);

	/* Wait for Voltage Regulator startup time (~20us) */
	delay_us(20);	// SysTick function

	// 4. ADC Calibration (Must be while ADEN=0)
	// Calibration measures the internal electrical offsets of the ADC. If the ADC is already "Enabled" (ADEN=1), the circuitry is active and noisy
	CLEAR_BIT(pADCHandle->pADCx->CR, ADC_CR_ADEN_POS);
	SET_BIT(pADCHandle->pADCx->CR, ADC_CR_ADCAL_POS);
	while(READ_BIT(pADCHandle->pADCx->CR, ADC_CR_ADCAL_POS));	// Keep reading this register bit if it's still 1. Only move to the next line once that bit flips to 0

	// 5. Configure Resolutions (RES bits in CFGR)
	pADCHandle->pADCx->CFGR &= ~(0x3 << 3);
	pADCHandle->pADCx->CFGR |= (pADCHandle->ADC_Config.ADC_Resolution << 3);

	// 6. Set Sampling Time for the specific channel
	if (pADCHandle->ADC_Config.ADC_Channel <= 9)
	{
		pADCHandle->pADCx->SMPR[0] &= ~(0x7 << (3 * pADCHandle->ADC_Config.ADC_Channel));
		pADCHandle->pADCx->SMPR[0] |= (pADCHandle->ADC_Config.ADC_SampleTime << (3 * pADCHandle->ADC_Config.ADC_Channel));
	} else
	{
		uint8_t shift = 3 * (pADCHandle->ADC_Config.ADC_Channel - 10);
		pADCHandle->pADCx->SMPR[1] &= ~(0x7 << shift);
		pADCHandle->pADCx->SMPR[1] |= (pADCHandle->ADC_Config.ADC_SampleTime << shift);
	}

	// 7. Sequence length and channel selection (1st conversion)
	pADCHandle->pADCx->SQR[0] &= ~(0xF); // L=0 (1 conversion)
	pADCHandle->pADCx->SQR[0] |= (pADCHandle->ADC_Config.ADC_Channel << 6);

	// 8. Enable ADC
	SET_BIT(pADCHandle->pADCx->CR, ADC_CR_ADEN_POS);
	while(!READ_BIT(pADCHandle->pADCx->ISR, ADC_ISR_ADRDY_POS));
	// After you set ADEN=1 (Enable), the ADC doesn't turn on instantly. It has to perform an internal "handshake."
	// Wait here and do nothing until the ADC hardware flips the ADRDY (ADC Ready) flag to 1
}



/*********************************************************************
 * @fn      		- ADC_DeInit
 *
 * @brief           - Resets the ADC peripheral registers
 *
 * @param[in]		- Base address of the ADC peripheral
 *
 * @return			- None
 *
 * @note			- Uses the RCC Reset register
 *********************************************************************/
void ADC_DeInit(ADC_RegDef_t *pADCx)
{
	ADC_REG_RESET();
}



/*********************************************************************
 * @fn      		- ADC_StartConversion
 *
 * @brief           - Triggers the start of an ADC conversion
 *
 * @param[in]		- Base address of the ADC peripheral
 *
 * @return			- None
 *
 * @note			- ADC must be enabled (ADEN=1) before calling this
 *********************************************************************/
void ADC_StartConversion(ADC_RegDef_t *pADCx)
{
	SET_BIT(pADCx->CR, ADC_CR_ADSTART_POS);
}



/*********************************************************************
 * @fn      		- ADC_GetResult
 *
 * @brief           - Waits for conversion to finish and returns the value
 *
 * @param[in]		- Base address of the ADC peripheral
 *
 * @return			- The 12-bit (or lower) converted digital value
 *
 * @note			- This is a blocking function (polls EOC flag)
 *********************************************************************/
uint32_t ADC_GetResult(ADC_RegDef_t *pADCx)
{
	// Wait for EOC (End of conversion) flag
	// ADC takes time to "sample" the voltage on your potentiometer and "calculate" the digital number
	while(!READ_BIT(pADCx->ISR, ADC_ISR_EOC_POS));	// This ensures to only read valid data

	// Return result from DR
	return pADCx->DR;
}
