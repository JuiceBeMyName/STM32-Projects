#include "stm32l47xxx_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);



/*********************************************************************
 * @fn      		- SPI_PeriClockControl
 *
 * @brief           - Enables or disables the peripheral clock for the
 * 					  given SPI port
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral (e.g., SPI1, SPI2)
 * @param[in]		- EnorDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- This function MUST be called to turn on the hardware
 * 					  clock in the RCC registers before trying to configure
 * 					  any SPI registers
 *********************************************************************/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN(); // Enables clock for SPI1 on APB2
		}
		else if (pSPIx == SPI2)
		{
			SPI2_PCLK_EN(); // Enables clock for SPI2 on APB1
		}
		else if (pSPIx == SPI3)
		{
			SPI3_PCLK_EN(); // Enables clock for SPI3 on APB1
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI(); // Disables clock for SPI1
		}
		else if (pSPIx == SPI2)
		{
			SPI2_PCLK_DI(); // Disables clock for SPI2
		}
		else if (pSPIx == SPI3)
		{
			SPI3_PCLK_DI(); // Disables clock for SPI3
		}
	}
}


/*
 * Init and De-init
 */

/*********************************************************************
 * @fn      		- SPI_Init
 *
 * @brief           - Initializes the SPI peripheral based on the user
 * configuration provided in the handle structure.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure which
 * contains the base address and configuration settings.
 *
 * @return			- none
 *
 * @note			- The peripheral clock must be enabled via
 * SPI_PeriClockControl() before calling this function.
 *********************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
    // SPI_CR1 register
	uint32_t tempreg = 0;

	// 1. Configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	// 2. Configure the bus config
	if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);

	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// bidi mode should be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);

	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);

		// rxonly bit should be set
		tempreg |= (1 << SPI_CR1_RXONLY);
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_TXONLY)
	{
		// bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		// rxonly bit should be cleared
		tempreg &= ~(1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the SPI serial clock speed (baud rate)
	tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

	// 4. Configure the CPOL (Clock Polarity)
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);

	// 5. Configure the CPHA (Clock Phase)
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

	// 6. Configure the SSM (Software Slave Management)
	tempreg |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

	// End of CR1 configuration
	// Write all the configured bits to the CR1 register
	pSPIHandle->pSPIx->CR1 = tempreg;

	// 7. Configure the DFF
	uint32_t tempreg_cr2 = 0;

	if (pSPIHandle->SPIConfig.SPI_DFF == SPI_DFF_8BITS)
	{
		// From Manual: 8-bit is '0111' (0x7)
		// Shift 0x7 to bit position 8
		tempreg_cr2 |= (0x7 << SPI_CR2_DS);
	} else if (pSPIHandle->SPIConfig.SPI_DFF == SPI_DFF_16BITS)
	{
		// From Manual: 16-bit is '1111' (0xF)
		// Shift 0xF to bit position 8
		tempreg_cr2 |= (0xF << SPI_CR2_DS);
	}

	// Write the configured bits to the actual CR2 register
	pSPIHandle->pSPIx->CR2 = tempreg_cr2;
}

/*********************************************************************
 * @fn      		- SPI_DeInit
 *
 * @brief           - De-initializes the SPI peripheral, forcing its
 * hardware registers back to their default reset states.
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 *
 * @return			- none
 *
 * @note			- This is typically done by setting the corresponding
 * reset bit in the RCC Peripheral Reset Register (RCC_APBxRSTR).
 *********************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if (pSPIx == SPI1)
		{
			SPI1_REG_RESET();
		}
	else if (pSPIx == SPI2)
		{
			SPI2_REG_RESET();
		}
	else if (pSPIx == SPI3)
		{
			SPI3_REG_RESET();
		}
}



/*********************************************************************
 * @fn      		- SPI_GetFlagStatus
 *
 * @brief           - Checks if a specific flag is set in the SPI
 * Status Register (SR).
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 * @param[in]		- FlagName: The macro representing the flag to check
 * (e.g., SPI_TXE_FLAG, SPI_RXNE_FLAG)
 *
 * @return			- uint8_t: FLAG_SET (1) or FLAG_RESET (0)
 *
 * @note			- This is a helper function heavily utilized by the
 * blocking Send and Receive APIs to monitor hardware state.
 *********************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if (pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}



/*
 * Data Send and Receive
 */

/*********************************************************************
 * @fn      		- SPI_SendData
 *
 * @brief           - Transmits data over the SPI bus (Blocking method).
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 * @param[in]		- pTxBuffer: Pointer to the array/buffer of data to send
 * @param[in]		- Len: Number of bytes to be transmitted
 *
 * @return			- none
 *
 * @note			- This is a blocking (polling) function. The CPU will
 * wait in a while loop until all bytes are pushed into
 * the TX buffer.
 *********************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	// If Data Size (DS) is configured for 16-bit (0xF)
	if ((pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
	{
		// The length MUST be an even number of bytes. If it's odd, reject the job!
		if (Len % 2 != 0)
		{
			return; // Silently reject the invalid transfer to prevent underflow
		}
	}

    while (Len > 0)
    {
    	// 1. Wait until TXE is set
    	while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);	// stay as long as the buffer is NOT empty

    	// 2. Check the DS (Data Size) bits in CR2
    	// If the 4 bits starting at SPI_CR2_DS equal 1111 (0xF), it's 16-bit
    	if ((pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
    	{
    		// 16 bit DFF
    		// 1. Load the data into the DR
    		// Load 2 bytes of data into the DR
    		pSPIx->DR = *((uint16_t*)pTxBuffer);

    		Len -= 2;	// decrement by 2 bytes
    		pTxBuffer += 2;	// move pointer by 2 bytes
    	} else
    	{
    		*((__vo uint8_t *) &pSPIx->DR) = *pTxBuffer;	// volatile needed cause compiler would delete all the writes to the same memory address like her DR address

    		Len--;	// decrement length by 1 byte
    		pTxBuffer++;	// Move the pointer forward by 1 byte
    	}
    }
}



/*********************************************************************
 * @fn      		- SPI_ReceiveData
 *
 * @brief           - Receives data from the SPI bus (Blocking method).
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 * @param[in]		- pRxBuffer: Pointer to the array/buffer where received
 * data will be stored
 * @param[in]		- Len: Number of bytes to be received
 *
 * @return			- none
 *
 * @note			- This is a blocking (polling) function. The CPU will
 * wait in a while loop until all expected bytes arrive
 * in the RX buffer.
 *********************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	// If Data Size (DS) is configured for 16-bit (0xF)
	if ((pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
	{
		// The length MUST be an even number of bytes. If it's odd, reject the job!
		if (Len % 2 != 0)
		{
			return; // Silently reject the invalid transfer to prevent memory corruption
		}
	}

    while (Len > 0)
    {
    	// 1. Wait until RXNE is set (Receive buffer is NOT empty)
    	while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET); // Stay while buffer IS empty

    	// 2. Check the DS (Data Size) bits in CR2 for STM32L4
    	// If the 4 bits starting at SPI_CR2_DS equal 1111 (0xF), it's 16-bit
    	if ((pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
    	{
    		// --- 16-BIT DATA FRAME FORMAT ---
    		// Load 2 bytes of data from DR into the RxBuffer
    		*((uint16_t*)pRxBuffer) = pSPIx->DR;

    		Len -= 2;          // Decrement length by 2 bytes
    		pRxBuffer += 2;    // Safely move pointer forward by 2 bytes
    	}
        else
    	{
    		// --- 8-BIT DATA FRAME FORMAT ---
            // Force the hardware to only pop 1 byte out of the receive FIFO
    		*pRxBuffer = *((__vo uint8_t *)&pSPIx->DR);

    		Len--;             // Decrement length by 1 byte
    		pRxBuffer++;       // Move the pointer forward by 1 byte
    	}
    }
}



/*********************************************************************
 * @fn      		- SPI_PeripheralControl
 *
 * @brief           - Enables or disables the SPI peripheral by toggling
 * the SPE (SPI Enable) bit in the CR1 register.
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 * @param[in]		- EnOrDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- The SPI should only be enabled AFTER all other
 * configuration settings (Baud rate, CPOL, CPHA, etc.)
 * have been written to the registers.
 *********************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	} else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}



/*********************************************************************
 * @fn      		- SPI_SSIConfig
 *
 * @brief           - Configures the Internal Slave Select (SSI) bit in
 * 					  the CR1 register
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 * @param[in]		- EnOrDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- This function only has an effect if Software Slave
 * 					  Management (SSM) is enabled. It drives the internal
 * 					  NSS signal high (1) or low (0)
 *********************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	} else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}



/*********************************************************************
 * @fn      		- SPI_SSOEConfig
 *
 * @brief           - Configures the SS Output Enable (SSOE) bit in
 * 					  the CR2 register
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 * @param[in]		- EnOrDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- Used when Hardware Slave Management is enabled.
 * 					  When enabled (1), the NSS pin is automatically
 * 					  driven low when the SPI peripheral is enabled.
 *********************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	} else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}



/*********************************************************************
 * @fn      		- SPI_SendDataIT
 *
 * @brief           - Initializes a non-blocking (interrupt-driven)
 * SPI data transmission
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 * @param[in]		- pTxBuffer: Pointer to the array/buffer to send
 * @param[in]		- Len: Number of bytes to be transmitted
 *
 * @return			- uint8_t: 0 for SUCCESS (Transfer accepted)
 * 1 for BUSY (Transfer rejected)
 *
 * @note			- This function sets up the state and enables the
 * TXE interrupt. The actual data transfer happens
 * in the background within the ISR
 *********************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if (state != SPI_BUSY_IN_TX)
	{
		// --- THE FIX: Validate 16-bit length ---
		// If Data Size (DS) is configured for 16-bit (0xF)
		if ((pSPIHandle->pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
		{
			// The length MUST be an even number of bytes. If it's odd, reject the job!
			if (Len % 2 != 0)
			{
				return 1; // Return 1 (Transfer Rejected) due to invalid length
			}
		}

		// 1. Save the Tx buffer address and Len information in global handle
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		// 2. Mark the SPI state as busy in transmission
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		// 3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

        return 0; // Return 0 to indicate SUCCESS (Transfer Accepted)
	}

	return 1; // Return 1 to indicate BUSY (Transfer Rejected)
}



/*********************************************************************
 * @fn      		- SPI_ReceiveDataIT
 *
 * @brief           - Initializes a non-blocking (interrupt-driven)
 * SPI data reception.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 * @param[in]		- pRxBuffer: Pointer to the buffer to store received data
 * @param[in]		- Len: Number of bytes to be received
 *
 * @return			- uint8_t: 0 for SUCCESS (Transfer accepted)
 * 1 for BUSY (Transfer rejected)
 *
 * @note			- This function sets up the state and enables the
 * RXNE interrupt. The actual data reception happens
 * in the background within the ISR.
 *********************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if (state != SPI_BUSY_IN_RX)
	{
		// --- THE FIX: Validate 16-bit length ---
		// If Data Size (DS) is configured for 16-bit (0xF)
		if ((pSPIHandle->pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
		{
			// The length MUST be an even number of bytes. If it's odd, reject the job!
			if (Len % 2 != 0)
			{
				return 1; // Return 1 (Transfer Rejected) due to invalid length
			}
		}

		// 1. Save the Rx buffer address and Len information in global handle
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		// 2. Mark the SPI state as busy in reception
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		// 3. Enable the RXNEIE control bit to get interrupt whenever RXNE flag is set
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

        return 0; // Return 0 to indicate SUCCESS (Transfer Accepted)
	}

	return 1; // Return 1 to indicate BUSY (Transfer Rejected)
}



/*********************************************************************
 * @fn      		- SPI_IRQHandling
 *
 * @brief           - Dispatches SPI interrupts to the appropriate worker
 * function based on which flag triggered the ISR.
 *
 * @param[in]		- pHandle: Pointer to the SPI handle structure
 *
 * @return			- none
 *
 * @note			- This function acts as the "traffic cop" and should
 * be called from the actual SPIx_IRQHandler()
 * inside your main application.
 *********************************************************************/
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	uint8_t temp1, temp2;
	// Check for TXE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if (temp1 && temp2)
	{
		// handle TXE
		spi_txe_interrupt_handle(pHandle);
	}

	// check for RXNE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if (temp1 && temp2)
	{
		// handle RXNE
		spi_rxne_interrupt_handle(pHandle);
	}

	// check for OVR flag
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if (temp1 && temp2)
	{
		// handle OVR error
		spi_ovr_err_interrupt_handle(pHandle);
	}
}



/*********************************************************************
 * @fn      		- SPI_IRQInterruptConfig
 *
 * @brief           - Configures the NVIC to enable or disable interrupts
 * for a specific IRQ number.
 *
 * @param[in]		- IRQNumber: IRQ number of the interrupt
 * @param[in]		- EnorDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *********************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			// program ISER0 register (Interrupt Set-Enable Register 0)
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			// program ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if (IRQNumber >= 64 && IRQNumber < 96)
		{
			// program ISER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 32));
		}
	}
	else
	{
		if (IRQNumber <= 31)
		{
			// program ICER0 register (Interrupt Clear-Enable Register 0)
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			// program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if (IRQNumber >= 64 && IRQNumber < 96)
		{
			// program ICER2 register
			*NVIC_ICER2 |= (1 << (IRQNumber % 32));
		}
	}
}



/*********************************************************************
 * @fn      		- SPI_IRQPriorityConfig
 *
 * @brief           - Configures the NVIC interrupt priority for a
 * specific IRQ number.
 *
 * @param[in]		- IRQNumber: IRQ number of the interrupt
 * @param[in]		- IRQPriority: Priority level to be set
 *
 * @return			- none
 *********************************************************************/
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. First let's find out the IPR (Interrupt Priority Register)
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber % 4;

	// 2. Calculate the shift amount.
    // The lower 4 bits are not implemented in STM32, so we shift by (8 - 4) = 4 bits
	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}



// Some helper function implementations
/*********************************************************************
 * @fn      		- spi_txe_interrupt_handle
 *
 * @brief           - Internal helper function to handle pushing the next
 * byte/word into the Data Register during an interrupt.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 *
 * @return			- none
 *
 * @note			- Automatically closes transmission when finished.
 *********************************************************************/
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// 2. Check the DS (Data Size) bits in CR2
	// If the 4 bits starting at SPI_CR2_DS equal 1111 (0xF), it's 16-bit
	if ((pSPIHandle->pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
	{
		// 16 bit DFF
		// 1. Load the data into the DR
		// Load 2 bytes of data into the DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);

		pSPIHandle->TxLen -= 2;	// decrement by 2 bytes
		pSPIHandle->pTxBuffer += 2;	// move pointer by 2 bytes
	} else
	{
		*((__vo uint8_t *) &pSPIHandle->pSPIx->DR) = *pSPIHandle->pTxBuffer;	// volatile needed cause compiler would delete all the writes to the same memory address like her DR address

		pSPIHandle->TxLen--;	// decrement length by 1 byte
		pSPIHandle->pTxBuffer++;	// Move the pointer forward by 1 byte
	}

	if (!pSPIHandle->TxLen)	// TxLen = 0 means entire message has been sent
	{
		// TxLen is zero, so close the SPI transmission and inform the application that TX is over
		// This prevents interrupts from setting up of the TXE flag
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}



/*********************************************************************
 * @fn      		- spi_rxne_interrupt_handle
 *
 * @brief           - Internal helper function to handle popping the next
 * byte/word out of the Data Register during an interrupt.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 *
 * @return			- none
 *
 * @note			- Automatically closes reception when finished.
 *********************************************************************/
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// 1. Check the DS (Data Size) bits in CR2 for STM32L4
	if ((pSPIHandle->pSPIx->CR2 & (0xF << SPI_CR2_DS)) == (0xF << SPI_CR2_DS))
	{
		// --- 16 BIT RECEIVE ---
		*((uint16_t *)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		pSPIHandle->pRxBuffer += 2; // FIXED: Move forward in memory
	}
    else
	{
		// --- 8 BIT RECEIVE ---
        // FIXED: Force hardware to only pop 1 byte out of the receive FIFO
		*pSPIHandle->pRxBuffer = *((__vo uint8_t *)&pSPIHandle->pSPIx->DR);
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;    // FIXED: Move forward in memory
	}

	if (!pSPIHandle->RxLen)
	{
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}



/*********************************************************************
 * @fn      		- spi_ovr_err_interrupt_handle
 *
 * @brief           - Internal helper function to clear an Overrun Error.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 *
 * @return			- none
 *
 * @note			- Triggers the application callback to inform the
 * user that data was permanently lost.
 *********************************************************************/
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp = 0;

	// 1. Clear the OVR flag
	// We only clear it automatically if we are not currently transmitting
	// If we're busy transmitting, the application will clear it later
	if (pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		// The STM32 hardware requires a read of DR followed by a read of SR to clear the OVR bit
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;	// This casts the variable to void so the compiler doesn't complain about an "unused variable"

	// 2. Inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}



/*********************************************************************
 * @fn      		- SPI_ClearOVRFlag
 *
 * @brief           - Manually clears the Overrun (OVR) flag.
 *
 * @param[in]		- pSPIx: Base address of the SPI peripheral
 *
 * @return			- none
 *
 * @note			- Exposed for application-level error recovery.
 *********************************************************************/
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}



/*********************************************************************
 * @fn      		- SPI_CloseTransmission
 *
 * @brief           - Safely aborts or finalizes an active SPI transmission.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 *
 * @return			- none
 *
 * @note			- Disables the TXE interrupt and resets the state.
 *********************************************************************/
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}



/*********************************************************************
 * @fn      		- SPI_CloseReception
 *
 * @brief           - Safely aborts or finalizes an active SPI reception.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 *
 * @return			- none
 *
 * @note			- Disables the RXNE interrupt and resets the state.
 *********************************************************************/
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}



/*********************************************************************
 * @fn      		- SPI_ApplicationEventCallback
 *
 * @brief           - Application-level callback function for SPI events.
 *
 * @param[in]		- pSPIHandle: Pointer to the SPI handle structure
 * @param[in]		- AppEv: The event macro (e.g., SPI_EVENT_TX_CMPLT)
 *
 * @return			- none
 *
 * @note			- This is a __weak implementation. The user is expected
 * to write their own version of this function inside
 * their main.c application code to react to events.
 *********************************************************************/
__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{

}
