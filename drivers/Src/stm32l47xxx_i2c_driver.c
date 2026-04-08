#include "stm32l47xxx_i2c_driver.h"


/*********************************************************************
 * @fn      		- I2C_PeriClockControl
 *
 * @brief           - Enables or disables the peripheral clock for the
 * 					  given I2C port
 *
 * @param[in]		- pI2Cx: Base address of the I2C peripheral (e.g., I2C1, I2C2)
 * @param[in]		- EnorDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- This function MUST be called to turn on the hardware
 * 					  clock in the RCC registers before trying to configure
 * 					  any I2C registers
 *********************************************************************/
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	else
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}



/*********************************************************************
 * @fn      		- I2C_PeripheralControl
 *
 * @brief           - Enables or disables the given I2C peripheral by
 * setting or clearing the PE (Peripheral Enable) bit
 * in the CR1 register
 *
 * @param[in]		- pI2Cx: Base address of the I2C peripheral (e.g., I2C1)
 * @param[in]		- EnOrDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- The STM32L4 hardware requires the peripheral to be
 * DISABLED while configuring certain registers like
 * TIMINGR and OAR1. Enable it only when you are
 * ready to start communication.
 *********************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
	else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}



/*********************************************************************
 * @fn      		- I2C_Init
 *
 * @brief           - Initializes the I2C peripheral according to the
 * specified parameters in the user's handle structure
 *
 * @param[in]		- pI2CHandle: Pointer to the handle structure containing
 * the base address and configuration settings
 *
 * @return			- none
 *
 * @note			- The TIMINGR hex values used in this function assume
 * a default system clock (SYSCLK) of 16 MHz.
 *********************************************************************/
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	// 1. Enable the clock for the I2C peripheral first
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	// 2. Configure the Clock Speed (TIMINGR register)
	// Note: These hex values are pre-calculated by ST for a 16 MHz system clock.
	// If you ever change your main clock speed, these values will need to be recalculated
	if (pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_SM)
	{
		pI2CHandle->pI2Cx->TIMINGR = 0x00404C74; // 100 kHz
	}
	else if (pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_FM4K)
	{
		pI2CHandle->pI2Cx->TIMINGR = 0x0010020A; // 400 kHz
	}
	else if (pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_FM2K)
	{
		pI2CHandle->pI2Cx->TIMINGR = 0x00200C28; // 200 kHz
	}
	else if (pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_FM_PLUS)
	{
		pI2CHandle->pI2Cx->TIMINGR = 0x00000105; // 1 MHz
	}

	// 3. Configure the Device Own Address (OAR1 Register)
	uint32_t tempreg = 0;
	// The 7-bit address must be shifted left by 1 (it occupies bits 7:1)
	tempreg |= (pI2CHandle->I2C_Config.I2C_DeviceAddress << 1);

	// Enable the Own Address 1 by setting the OA1EN bit (Bit 15)
	tempreg |= (1 << I2C_OAR1_OA1EN);
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	// 4. Configure the ACK control	(CR2 Register)
	// On STM32L4, hardware ACKs automatically by default
	// To DISABLE ACKs, we have to SET the NACK bit (Bit 15)
	if (pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE)
	{
		pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_NACK);	// Clear NACK bit (ACKs are sent normally)
	}
	else
	{
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_NACK);	// Set NACK bit (NACKs are sent instead)
	}
}



/*********************************************************************
 * @fn      		- I2C_DeInit
 *
 * @brief           - Resets the given I2C peripheral back to its
 * factory default state
 *
 * @param[in]		- pI2Cx: Base address of the I2C peripheral (e.g., I2C1)
 *
 * @return			- none
 *
 * @note			- This creates a hardware reset pulse using the RCC
 * APB1RSTR1 register (pressing and releasing the reset button).
 *********************************************************************/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		RCC->APB1RSTR1 |= (1 << 21);	// 1. Set the reset bit
		RCC->APB1RSTR1 &= ~(1 << 21);	// 2. Clear the reset bit to finish
	}
	else if (pI2Cx == I2C2)
	{
		RCC->APB1RSTR1 |= (1 << 22);	// It's like holding reset button
		RCC->APB1RSTR1 &= ~(1 << 22);	// It's like releasing that button, so everything is correctly reset
	}
	else if (pI2Cx == I2C3)
	{
		RCC->APB1RSTR1 |= (1 << 23);
		RCC->APB1RSTR1 &= ~(1 << 23);
	}
}



/*********************************************************************
 * @fn      		- I2C_MasterSendData
 *
 * @brief           - Transmits data as a Master device over the I2C bus
 * using a blocking (polling) method
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 * @param[in]		- pTxBuffer: Pointer to the buffer containing data to send
 * @param[in]		- Len: Number of bytes to transmit (Assumes <= 255)
 * @param[in]		- SlaveAddr: The 7-bit address of the target slave device
 *
 * @return			- none
 *
 * @note			- This uses the STM32 V2 hardware "flight plan" method
 * in the CR2 register. Address phase generation is automated
 *********************************************************************/
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr)
{
	// The hardware NBYTES field is only 8 bits wide (Max 255).
	if (Len == 0 || Len > 255)
	{
		return; // Silently reject the invalid transfer
	}

	uint32_t tempreg = pI2CHandle->pI2Cx->CR2;

	// 1. Clear out the bits we are about to configure so we start fresh
	// Clear Slave Address (Bits 9:0), Number of Bytes (Bits 23:16), and RD_WRN bit (Bit 10)
	tempreg &= ~(0x3FF << I2C_CR2_SADD);	// 10 bits of ones
	tempreg &= ~(0xFF << I2C_CR2_NBYTES);
	tempreg &= ~(1 << I2C_CR2_RD_WRN);

	// 2. Set the Slave Address
	// The 7-bit address must be shifted left by 1 to sit in SADD[7:1]
	tempreg |= (SlaveAddr << 1) << I2C_CR2_SADD;	// Bit 0 is reserved Read/Write signal

	// 3. Set the Number of Bytes to transmit
	// Note: For this basic driver, we assume Len is 255 bytes or less
	tempreg |= (Len << I2C_CR2_NBYTES);

	// 4. Request a WRITE operation (0 = Write, 1 = Read)
	// Since we cleared this bit above, it is already 0, but we can be explicit
	tempreg &= ~(1 << I2C_CR2_RD_WRN);

	// 5. Set the START bit to begin the communication
	tempreg |= (1 << I2C_CR2_START);

	// Write our "flight plan" back to the actual CR2 hardware register
	pI2CHandle->pI2Cx->CR2 = tempreg;

	// 6. Loop until all bytes are sent
	while (Len > 0)
	{
		// Wait until the TXIS (Transmit Interrupt Status) flag is set in ISR
		// TXIS means "The TXDR register is empty, give me the next byte!"
		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXIS));

		// Write data to the TXDR (Transmit Data Register)
		pI2CHandle->pI2Cx->TXDR = *pTxBuffer;

		// Move to the next byte in the buffer and decrease the length
		pTxBuffer++;
		Len--;
	}

	// 7. Wait for the TC (Transfer Complete) flag in ISR
	// TC means "I have successfully sent the exact NBYTES you told me to send"
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TC));

	// 8. Generate the STOP condition to release the I2C bus
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_STOP);
}



/*********************************************************************
 * @fn      		- I2C_GetFlagStatus
 *
 * @brief           - Checks whether a specific I2C status flag is set
 * or reset in the Interrupt & Status Register (ISR)
 *
 * @param[in]		- pI2Cx: Base address of the I2C peripheral
 * @param[in]		- FlagName: Macro of the specific flag to check
 *
 * @return			- FLAG_SET (1) or FLAG_RESET (0)
 *
 * @note			- none
 *********************************************************************/
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if (pI2Cx->ISR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}



/*********************************************************************
 * @fn      		- I2C_MasterReceiveData
 *
 * @brief           - Receives data as a Master device over the I2C bus
 * using a blocking (polling) method
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 * @param[in]		- pRxBuffer: Pointer to the buffer to store received data
 * @param[in]		- Len: Number of bytes to receive (Assumes <= 255)
 * @param[in]		- SlaveAddr: The 7-bit address of the target slave device
 *
 * @return			- none
 *
 * @note			- The V2 hardware automatically handles NACKing the
 * final byte based on the NBYTES configuration.
 *********************************************************************/
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr)
{
	// The hardware NBYTES field is only 8 bits wide (Max 255).
	// Advanced drivers use the RELOAD bit for >255, but we restrict it here.
	if (Len == 0 || Len > 255)
	{
		return; // Silently reject the invalid transfer
	}

	uint32_t tempreg = pI2CHandle->pI2Cx->CR2;

	// 1. Clear out the bits we're about to configure
	// Clear Slave Address (Bits 9:0), Number of Bytes (Bits 23:16), and RD_WRN bit (Bit 10)
	tempreg &= ~(0x3FF << I2C_CR2_SADD);
	tempreg &= ~(0xFF << I2C_CR2_NBYTES);
	tempreg &= ~(1 << I2C_CR2_RD_WRN);

	// 2. Set the Slave Address
	tempreg |= (SlaveAddr << 1) << I2C_CR2_SADD;

	// 3. Set the Number of Bytes to receive
	tempreg |= (Len << I2C_CR2_NBYTES);

	// 4. Request a READ operation (0 = Write, 1 = Read)
	// This is the main difference from the Send function
	tempreg |= (1 << I2C_CR2_RD_WRN);

	// 5. Set the START bit to begin the communication
	tempreg |= (1 << I2C_CR2_START);

	// Write our "flight plan" back to the hardware
	pI2CHandle->pI2Cx->CR2 = tempreg;

	// 6. Loop until all bytes are received
	while (Len > 0)
	{
		// Wait until the RXNE (Receive Data Register Not Empty) flag is set
		// RXNE means "I just got a byte from the sensor, come pick it up!"
		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		// Read the data from the RXDR (Receive Data Register)
		// Doing this automatically clears the RXNE flag in the hardware
		*pRxBuffer = pI2CHandle->pI2Cx->RXDR;

		// Move to the next byte in the buffer
		pRxBuffer++;
		Len--;
	}

	// 7. Wait for the TC (Transfer Complete) flag
	// The hardware just automatically sent a NACK to the sensor
	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TC));

	// 8. Generate the STOP condition to release the I2C bus
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_STOP);
}



/*********************************************************************
 * @fn      		- I2C_MasterSendDataIT
 *
 * @brief           - Transmits data as a Master device over the I2C bus
 * using a non-blocking (interrupt) method
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 * @param[in]		- pTxBuffer: Pointer to the buffer containing data to send
 * @param[in]		- Len: Number of bytes to transmit
 * @param[in]		- SlaveAddr: The 7-bit address of the target slave device
 *
 * @return			- uint8_t: Returns the state of the driver (I2C_READY, I2C_BUSY_IN_TX)
 *
 * @note			- This function returns immediately. The actual transmission
 * happens in the background via interrupts.
 *********************************************************************/
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr)
{
	uint8_t state = pI2CHandle->TxRxState;

	if (state == I2C_READY)
	{
		// 1. Save the variables into the handle so the ISR can find them later
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;

		// 2. Prepare the CR2 "Flight Plan" (Exactly like the polling function)
		uint32_t tempreg = pI2CHandle->pI2Cx->CR2;
		tempreg &= ~(0x3FF << I2C_CR2_SADD);
		tempreg &= ~(0xFF << I2C_CR2_NBYTES);
		tempreg &= ~(1 << I2C_CR2_RD_WRN);

		tempreg |= (SlaveAddr << 1) << I2C_CR2_SADD;
		tempreg |= (Len << I2C_CR2_NBYTES);
		tempreg |= (1 << I2C_CR2_START);
		pI2CHandle->pI2Cx->CR2 = tempreg;

		// 3. Enable the necessary Interrupts in CR1
		// TXIE: Triggers the ISR when TXIS goes HIGH (Ready for next byte)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_TXIE);
		// TCIE: Triggers the ISR when TC goes HIGH (Transfer completely finished)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_TCIE);
		// ERRIE: Triggers the ISR if an Error happens (BERR, ARLO, etc.)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_ERRIE);

		return 0; // Return 0 (SUCCESS - Job Accepted)
	}

	return 1; // Return 1 (BUSY - Job Rejected)
}



/*********************************************************************
 * @fn      		- I2C_MasterReceiveDataIT
 *
 * @brief           - Receives data as a Master device over the I2C bus
 * using a non-blocking (interrupt) method
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 * @param[in]		- pRxBuffer: Pointer to the buffer to store received data
 * @param[in]		- Len: Number of bytes to receive
 * @param[in]		- SlaveAddr: The 7-bit address of the target slave device
 *
 * @return			- uint8_t: Returns the state of the driver (I2C_READY, I2C_BUSY_IN_RX)
 *
 * @note			- This function returns immediately. The actual reception
 * happens in the background via interrupts.
 *********************************************************************/
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr)
{
	uint8_t state = pI2CHandle->TxRxState;

	if (state == I2C_READY)
	{
		// 1. Save the variables into the handle
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len;
		pI2CHandle->DevAddr = SlaveAddr;

		// 2. Prepare the CR2 "Flight Plan"
		uint32_t tempreg = pI2CHandle->pI2Cx->CR2;
		tempreg &= ~(0x3FF << I2C_CR2_SADD);
		tempreg &= ~(0xFF << I2C_CR2_NBYTES);
		tempreg &= ~(1 << I2C_CR2_RD_WRN);

		tempreg |= (SlaveAddr << 1) << I2C_CR2_SADD;
		tempreg |= (Len << I2C_CR2_NBYTES);
		tempreg |= (1 << I2C_CR2_RD_WRN); // Set to READ mode
		tempreg |= (1 << I2C_CR2_START);
		pI2CHandle->pI2Cx->CR2 = tempreg;

		// 3. Enable the necessary Interrupts in CR1
		// RXIE: Triggers the ISR when RXNE goes HIGH (Data is waiting to be read)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_RXIE);
		// TCIE: Triggers the ISR when TC goes HIGH (Transfer completely finished)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_TCIE);
		// ERRIE: Triggers the ISR if an Error happens
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_ERRIE);

		return 0; // Return 0 (SUCCESS - Job Accepted)
	}

	return 1; // Return 1 (BUSY - Job Rejected)
}



/*********************************************************************
 * @fn      		- I2C_EV_IRQHandling
 *
 * @brief           - Handles I2C event interrupts (TXIS, RXNE, TC)
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 *
 * @return			- none
 *
 * @note			- This function must be called from the I2Cx_EV_IRQHandler
 * in your main application.
 *********************************************************************/
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	// 1. Handle TXIS (Hardware wants the next byte to transmit)
	if (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXIS))
	{
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			if (pI2CHandle->TxLen > 0)
			{
				pI2CHandle->pI2Cx->TXDR = *(pI2CHandle->pTxBuffer);
				pI2CHandle->TxLen--;
				pI2CHandle->pTxBuffer++;
			}
		}
	}

	// 2. Handle RXNE (Hardware has a new byte ready to be read)
	else if (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE))
	{
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if (pI2CHandle->RxLen > 0)
			{
				*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->RXDR;
				pI2CHandle->RxLen--;
				pI2CHandle->pRxBuffer++;
			}
		}
	}

	// 3. Handle TC (Transfer Complete - The NBYTES flight plan is finished)
	else if (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TC))
	{
		// Generate STOP condition if Repeated Start is disabled
		if (pI2CHandle->Sr == DISABLE)
		{
			pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_STOP);
		}

		// Save the state before we reset it so we know which callback to trigger
		uint8_t previousState = pI2CHandle->TxRxState;

		// Reset the handle state back to READY
		pI2CHandle->TxRxState = I2C_READY;

		// Turn off the interrupts
		pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_TXIE);
		pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_RXIE);
		pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_TCIE);

		// Notify the application
		if (previousState == I2C_BUSY_IN_TX)
		{
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
		}
		else if (previousState == I2C_BUSY_IN_RX)
		{
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
		}
	}
}



/*********************************************************************
 * @fn      		- I2C_ER_IRQHandling
 *
 * @brief           - Handles I2C error interrupts (BERR, ARLO, OVR)
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 *
 * @return			- none
 *
 * @note			- This function must be called from the I2Cx_ER_IRQHandler
 * in your main application. It automatically clears the errors.
 *********************************************************************/
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t isr_status = pI2CHandle->pI2Cx->ISR;

	// 1. Bus Error (BERR)
	if (isr_status & (1 << I2C_ISR_BERR))
	{
		// Clear the BERR flag by writing 1 to the ICR register
		pI2CHandle->pI2Cx->ICR |= (1 << I2C_ISR_BERR);

		// Notify the application that the bus crashed
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
	}

	// 2. Arbitration Loss (ARLO)
	if (isr_status & (1 << I2C_ISR_ARLO))
	{
		// Clear the ARLO flag
		pI2CHandle->pI2Cx->ICR |= (1 << I2C_ISR_ARLO);

		// Notify the application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
	}

	// 3. Overrun/Underrun (OVR)
	if (isr_status & (1 << I2C_ISR_OVR))
	{
		// Clear the OVR flag
		pI2CHandle->pI2Cx->ICR |= (1 << I2C_ISR_OVR);

		// Notify the app
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
	}
}



/*********************************************************************
 * @fn      		- I2C_IRQInterruptConfig
 *
 * @brief           - Configures the NVIC to enable or disable a specific IRQ
 *
 * @param[in]		- IRQNumber: The IRQ number to configure (e.g., IRQ_NO_I2C1_EV)
 * @param[in]		- EnorDi: ENABLE or DISABLE
 *
 * @return			- none
 *********************************************************************/
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			// Program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			// Program ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if (IRQNumber >= 64 && IRQNumber < 96)
		{
			// Program ISER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 32));
		}
	}
	else
	{
		if (IRQNumber <= 31)
		{
			// Program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			// Program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if (IRQNumber >= 64 && IRQNumber < 96)
		{
			// Program ICER2 register
			*NVIC_ICER2 |= (1 << (IRQNumber % 32));
		}
	}
}



/*********************************************************************
 * @fn      		- I2C_IRQPriorityConfig
 *
 * @brief           - Configures the priority of a specific IRQ in the NVIC
 *
 * @param[in]		- IRQNumber: The IRQ number to configure
 * @param[in]		- IRQPriority: The priority level (0 to 15)
 *
 * @return			- none
 *********************************************************************/
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. Find out which IPR register to use (IPR0 to IPR59)
	uint8_t iprx = IRQNumber / 4;

	// 2. Find out which section of the 32-bit register to use (0, 1, 2, or 3)
	uint8_t iprx_section = IRQNumber % 4;

	// 3. Calculate the shift amount. The lower 4 bits of each 8-bit section are not implemented in STM32
	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	// 4. Clear the 4 bits, then set the new priority
	*(NVIC_PR_BASE_ADDR + iprx) &= ~(0xF << shift_amount);
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}



/*********************************************************************
 * @fn      		- I2C_CloseSendData
 *
 * @brief           - Disables interrupt control bits and resets the handle state
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 *
 * @return			- none
 *********************************************************************/
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	// 1. Turn off the transmission and error interrupts
	pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_TXIE);
	pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_TCIE);
	pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_ERRIE);

	// 2. Unlock the driver so the application can try again
	pI2CHandle->TxRxState = I2C_READY;
}



/*********************************************************************
 * @fn      		- I2C_CloseReceiveData
 *
 * @brief           - Disables interrupt control bits and resets the handle state
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 *
 * @return			- none
 *********************************************************************/
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	// 1. Turn off the reception and error interrupts
	pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_RXIE);
	pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_TCIE);
	pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_ERRIE);

	// 2. Unlock the driver so the application can try again
	pI2CHandle->TxRxState = I2C_READY;
}



/*********************************************************************
 * @fn      		- I2C_SlaveSendData
 *
 * @brief           - Sends one byte of data when requested by the Master
 *
 * @param[in]		- pI2C: Base address of the I2C peripheral (e.g., I2C1)
 * @param[in]		- data: The 1-byte data to be transmitted
 *
 * @return			- none
 *
 * @note			- The slave does not generate the clock or manage the
 * transfer length. It just drops the byte into the TXDR.
 *********************************************************************/
void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data)
{
	// Drop the 1 byte of data directly into the Transmit Register
	pI2C->TXDR = data;
}



/*********************************************************************
 * @fn      		- I2C_SlaveReceiveData
 *
 * @brief           - Reads one byte of data sent by the Master
 *
 * @param[in]		- pI2C: Base address of the I2C peripheral (e.g., I2C1)
 *
 * @return			- uint8_t: The 1-byte data received from the Master
 *
 * @note			- none
 *********************************************************************/
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C)
{
	// Read the 1 byte of data directly from the Receive Register
	return (uint8_t)pI2C->RXDR;
}



/*********************************************************************
 * @fn      		- I2C_SlaveEnableDisableCallbackEvents
 *
 * @brief           - Enables or disables I2C interrupts for Slave mode
 *
 * @param[in]		- pI2Cx: Base address of the I2C peripheral
 * @param[in]		- EnorDi: ENABLE or DISABLE
 *
 * @return			- none
 *
 * @note			- STM32L4 (V2) uses CR1 for interrupt enables.
 *********************************************************************/
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_ADDRIE);
		pI2Cx->CR1 |= (1 << I2C_CR1_RXIE);
		pI2Cx->CR1 |= (1 << I2C_CR1_TXIE);
		pI2Cx->CR1 |= (1 << I2C_CR1_STOPIE);
		pI2Cx->CR1 |= (1 << I2C_CR1_ERRIE);
	}
	else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ADDRIE);
		pI2Cx->CR1 &= ~(1 << I2C_CR1_RXIE);
		pI2Cx->CR1 &= ~(1 << I2C_CR1_TXIE);
		pI2Cx->CR1 &= ~(1 << I2C_CR1_STOPIE);
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ERRIE);
	}
}



/*********************************************************************
 * @fn      		- I2C_ApplicationEventCallback
 *
 * @brief           - Application callback function to handle I2C events
 * and errors generated by the background interrupts
 *
 * @param[in]		- pI2CHandle: Pointer to the I2C handle structure
 * @param[in]		- AppEv: The application event macro (e.g., I2C_EV_TX_CMPLT)
 *
 * @return			- none
 *
 * @note			- This is a __weak implementation. The user application
 * should override this function in main.c to handle events.
 *********************************************************************/
__weak void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
	// This is a weak implementation. The application may override this function.
	// We leave this completely empty on purpose!
}
