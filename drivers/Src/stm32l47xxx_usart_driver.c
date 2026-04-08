#include "stm32l47xxx_usart_driver.h"

/*********************************************************************
 * @fn      		- USART_PeriClockControl
 *
 * @brief           - Enables or disables the peripheral clock for the given USART port
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral (e.g., USART1, USART2)
 * @param[in]		- EnorDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *
 * @note			- USART1 is on the APB2 bus. USART2 and USART3 are on the APB1 bus.
 *********************************************************************/
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (pUSARTx == USART1)
		{
			USART1_PCLK_EN();
		}
		else if (pUSARTx == USART2)
		{
			USART2_PCLK_EN();
		}
		else if (pUSARTx == USART3)
		{
			USART3_PCLK_EN();
		}
	}
	else
	{
		if (pUSARTx == USART1)
		{
			USART1_PCLK_DI();
		}
		else if (pUSARTx == USART2)
		{
			USART2_PCLK_DI();
		}
		else if (pUSARTx == USART3)
		{
			USART3_PCLK_DI();
		}
	}
}



/*********************************************************************
 * @fn      		- USART_Init
 *
 * @brief           - Initializes the USART peripheral based on the handle configuration
 *
 * @param[in]		- pUSARTHandle: Pointer to the USART handle structure
 *
 * @return			- none
 *********************************************************************/
void USART_Init(USART_Handle_t *pUSARTHandle)
{
	// Temporary register to hold our settings before we drop them into the hardware
	uint32_t tempreg = 0;

	// 1. Enable the Peripheral Clock for this specific USART
	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

	// ==========================================================
	// CONFIGURATION OF CR1 (Mode, Word Length, Parity)
	// ==========================================================

	// 1. Enable USART TX and RX engines according to the USART_Mode configuration item
	if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		// The code to enable the Receiver bit field
		tempreg |= (1 << USART_CR1_RE);


	}
	else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		// The code to enable the Transmitter bit field
		tempreg |= (1 << USART_CR1_TE);
	}
	else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		// The code to enable the both Transmitter and Receiver bit fields
		tempreg |= ((1 << USART_CR1_RE) | (1 << USART_CR1_TE));
	}

	// 2. Configure the Word Length (M0 is Bit 12, M1 is Bit 28)
	// M[1:0] literally just means: [ M1 , M0 ]
	if (pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_8BITS)
	{
		tempreg &= ~(1 << USART_CR1_M0);
		tempreg &= ~(1 << USART_CR1_M1);
	}
	else if (pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
	{
		tempreg |= (1 << USART_CR1_M0);
		tempreg &= ~(1 << USART_CR1_M1);
	}
	else if (pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_7BITS)
	{
		tempreg &= ~(1 << USART_CR1_M0);
		tempreg |= (1 << USART_CR1_M1);
	}

	// 3. Configure Parity Control
	if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		// Enable Parity Control (PCE) and keep Parity Selection (PS) at 0 for EVEN
		tempreg |= (1 << USART_CR1_PCE);
		tempreg &= ~(1 << USART_CR1_PS);
	}
	else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
	{
		// Enable Parity Control (PCE) and set Parity Selection (PS) to 1 for ODD
		tempreg |= (1 << USART_CR1_PCE);
		tempreg |= (1 << USART_CR1_PS);
	}

	// 4. Configure Oversampling Mode
	if (pUSARTHandle->USART_Config.USART_Oversampling == USART_OVERSAMPLING_8)
	{
		tempreg |= (1 << USART_CR1_OVER8);
	}
	else
	{
		tempreg &= ~(1 << USART_CR1_OVER8);
	}

	// Write all into CR1
	pUSARTHandle->pUSARTx->CR1 = tempreg;

	// ==========================================================
	// CONFIGURATION OF CR2 (Stop Bits)
	// ==========================================================
	tempreg = 0; 	// Reset tempreg for CR2

	// Program the number of stop bits into the STOP bits field of CR2
	tempreg |= (pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP);

	// Write it into CR2
	pUSARTHandle->pUSARTx->CR2 = tempreg;

	// ==========================================================
	// CONFIGURATION OF CR3 (Hardware Flow Control)
	// ==========================================================
	tempreg = 0;	// Reset tempreg for CR3

	if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		// CTS - clear to send. If CTS is LOW other device is ready if HIGH then busy/full
		tempreg |= (1 << USART_CR3_CTSE);
	}
	else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		// RTS - request to send ("I am ready to receive"). RTS is pulled LOW if it's full then RTS is pulled HIGH
		tempreg |= (1 << USART_CR3_RTSE);
	}
	else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		tempreg |= (1 << USART_CR3_CTSE);
		tempreg |= (1 << USART_CR3_RTSE);
	}

	// Write all to CR3
	pUSARTHandle->pUSARTx->CR3 = tempreg;


	// ==========================================================
	// CONFIGURATION OF BRR (Baud Rate)
	// ==========================================================
	// Function to do the math and drop it into the BRR register
	USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config.USART_Baud);
}



/*********************************************************************
 * @fn      		- USART_DeInit
 *
 * @brief           - Resets the USART peripheral back to its factory default state
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral to reset
 *
 * @return			- none
 *********************************************************************/
void USART_DeInit(USART_RegDef_t *pUSARTx)
{
	if (pUSARTx == USART1)
	{
		RCC->APB2RSTR |= (1 << 14);  // Force reset
		RCC->APB2RSTR &= ~(1 << 14); // Release reset
	}
	else if (pUSARTx == USART2)
	{
		RCC->APB1RSTR1 |= (1 << 17);
		RCC->APB1RSTR1 &= ~(1 << 17);
	}
	else if (pUSARTx == USART3)
	{
		RCC->APB1RSTR1 |= (1 << 18);
		RCC->APB1RSTR1 &= ~(1 << 18);
	}
}



/*********************************************************************
 * @fn      		- USART_SetBaudRate
 *
 * @brief           - Calculates and configures the Baud Rate register (BRR)
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral
 * @param[in]		- BaudRate: The desired baud rate (e.g., 9600, 115200)
 *
 * @return			- none
 *********************************************************************/
void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
	uint32_t PCLKx;
	uint32_t usartdiv;
	uint32_t tempreg = 0;

	// 1. Find the Peripheral Clock Frequency dynamically!
	if (pUSARTx == USART1)
	{
		// USART1 is on the fast APB2 bus
		PCLKx = RCC_GetPCLK2Value();
	}
	else
	{
		// USART2, USART3, UART4, UART5 are on the APB1 bus
		PCLKx = RCC_GetPCLK1Value();
	}

	// 2. Check if the user turned ON Oversampling by 8 (OVER8 bit in CR1)
	if (pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	{
		// --- OVER8 = 1 (Oversampling by 8) ---
		// The math formula requires multiplying the clock by 2
		usartdiv = ((2 * PCLKx) + (BaudRate / 2)) / BaudRate;

		// The STM32 hardware requires a strange bit-shift for the bottom 4 bits in OVER8 mode
		tempreg = ((usartdiv & 0xFFF0) | ((usartdiv & 0x000F) >> 1));
		pUSARTx->BRR = tempreg;
	}
	else
	{
		// --- OVER8 = 0 (Oversampling by 16) ---
		// This is the standard, clean math you already wrote!
		usartdiv = (PCLKx + (BaudRate / 2)) / BaudRate;
		pUSARTx->BRR = usartdiv;
	}
}



/*********************************************************************
 * @fn      		- USART_GetFlagStatus
 *
 * @brief           - Checks whether a specific USART flag is set or not
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral
 * @param[in]		- FlagName: The macro of the flag to check (e.g., USART_FLAG_TXE)
 *
 * @return			- uint8_t: SET (1) or RESET (0)
 *********************************************************************/
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName)
{
	if (pUSARTx->ISR & FlagName)
	{
		return SET;
	}

	return RESET;
}



/*********************************************************************
 * @fn      		- USART_SendData
 *
 * @brief           - Blocks the CPU and transmits data over the TX pin
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral
 * @param[in]		- pTxBuffer: Pointer to the data payload you want to send
 * @param[in]		- Len: Number of bytes to send
 *
 * @return			- none
 *********************************************************************/
void USART_SendData(USART_RegDef_t *pUSARTx, uint8_t *pTxBuffer, uint32_t Len)
{
	uint16_t *pdata;

	// Loop over the length of the data
	for (uint32_t i = 0; i < Len; i++)
	{
		// 1. Wait until TXE (Transmit Data Register Empty) flag is set
		// This traps the CPU here until the hardware is ready for the next byte
		while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

		// 2. Check the Word Length configuration
		// Extract the M0 and M1 bits from CR1 to see how we should package the data
		uint8_t val_M0 = (pUSARTx->CR1 >> USART_CR1_M0) & 0x1;
		uint8_t val_M1 = (pUSARTx->CR1 >> USART_CR1_M1) & 0x1;

		if (val_M0 == 1 && val_M1 == 0)
		{
			// --- 9-BIT MODE ---
			pdata = (uint16_t*) pTxBuffer;

			// Load exactly 9 bits of data into the TDR register
			pUSARTx->TDR = (*pdata & (uint16_t)0x01FF);

			// Check if Parity Control is enabled (PCE bit in CR1)
			if ((pUSARTx->CR1 >> USART_CR1_PCE) & 0x1)
			{
				// Parity is ON: Hardware automatically generates the 9th bit.
				// We only sent 8 bits of actual user data, so move buffer by 1 byte
				pTxBuffer++;
			}
			else
			{
				// Parity is OFF: All 9 bits are user data!
				// 9 bits take up two bytes of our buffer, so move buffer by 2 bytes
				pTxBuffer++;
				pTxBuffer++;
				i++; // We consumed an extra byte of 'Len', so increment our loop counter
			}

		}
		else
		{
			// --- 8-BIT OR 7-BIT MODE ---
			// We just drop standard 8-bit bytes into the TDR register
			pUSARTx->TDR = (*pTxBuffer & (uint8_t)0xFF);

			// Move buffer pointer forward by 1 byte
			pTxBuffer++;
		}
	}

	// 3. Wait for TC (Transmission Complete) flag to be set
	// This ensures the very last byte has completely left the shift register
	// before the function finishes and returns control to main.c
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}



/*********************************************************************
 * @fn      		- USART_ReceiveData
 *
 * @brief           - Blocks the CPU and receives data from the RX pin
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral
 * @param[in]		- pRxBuffer: Pointer to the memory where received data will be stored
 * @param[in]		- Len: Number of bytes to receive
 *
 * @return			- none
 *********************************************************************/
void USART_ReceiveData(USART_RegDef_t *pUSARTx, uint8_t *pRxBuffer, uint32_t Len)
{
	// Loop over the length of the data we expect to receive
	for (uint32_t i = 0; i < Len; i++)
	{
		// 1. Wait until RXNE (Receive Data Register Not Empty) flag is set
		// This traps the CPU here until the hardware confirms a new byte has arrived!
		while (USART_GetFlagStatus(pUSARTx, USART_FLAG_RXNE) == RESET);

		// 2. Check the Word Length configuration
		// Extract the M0 and M1 bits from CR1 to see how we should read the data
		uint8_t val_M0 = (pUSARTx->CR1 >> USART_CR1_M0) & 0x1;
		uint8_t val_M1 = (pUSARTx->CR1 >> USART_CR1_M1) & 0x1;

		if (val_M0 == 1 && val_M1 == 0)
		{
			// --- 9-BIT MODE ---
			// Check if Parity Control is enabled (PCE bit)
			if ((pUSARTx->CR1 >> USART_CR1_PCE) & 0x1)
			{
				// Parity is ON: The 9th bit is a parity bit we don't want
				// We only read the first 8 bits of actual user data
				*pRxBuffer = (pUSARTx->RDR & (uint8_t)0xFF);
				pRxBuffer++;
			}
			else
			{
				// Parity is OFF: All 9 bits are valid user data!
				// 9 bits require two bytes of memory space, so we cast to a 16-bit pointer
				*((uint16_t*) pRxBuffer) = (pUSARTx->RDR & (uint16_t)0x01FF);
				pRxBuffer++;
				pRxBuffer++;
				i++; // We filled two bytes of the user's length request, so increment loop
			}
		}
		else
		{
			// --- 8-BIT OR 7-BIT MODE ---
			// Check if Parity Control is enabled
			if ((pUSARTx->CR1 >> USART_CR1_PCE) & 0x1)
			{
				// Parity is ON: The 8th bit is parity. We only want 7 bits of user data.
				// 0x7F in binary is 0111 1111 (Perfect mask to grab only 7 bits)
				*pRxBuffer = (pUSARTx->RDR & (uint8_t)0x7F);
				pRxBuffer++;
			}
			else
			{
				// Parity is OFF: All 8 bits are valid user data!
				// 0xFF in binary is 1111 1111
				*pRxBuffer = (pUSARTx->RDR & (uint8_t)0xFF);
				pRxBuffer++;
			}
		}
	}
}



/*********************************************************************
 * @fn      		- USART_PeripheralControl
 *
 * @brief           - Enables or disables the USART peripheral engine
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral
 * @param[in]		- EnOrDi: ENABLE or DISABLE macro
 *
 * @return			- none
 *********************************************************************/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		// Turn the USART ON by setting the UE (USART Enable) bit to 1
		pUSARTx->CR1 |= (1 << USART_CR1_UE);
	}
	else
	{
		// Turn the USART OFF by clearing the UE bit to 0
		pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
	}
}



/*********************************************************************
 * @fn      		- USART_SendDataIT
 *
 * @brief           - Initiates a non-blocking transmission using Interrupts
 *
 * @param[in]		- pUSARTHandle: Pointer to the USART handle structure
 * @param[in]		- pTxBuffer: Pointer to the data payload you want to send
 * @param[in]		- Len: Number of bytes to send
 *
 * @return			- uint8_t: Returns the current state (USART_READY or USART_BUSY_IN_TX)
 *********************************************************************/
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t txstate = pUSARTHandle->TxBusyState;

	if (txstate != USART_BUSY_IN_TX)
	{
		// 1. Save the Tx buffer address and length information in the handle
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxLen = Len;

		// 2. Update the USART state to BUSY
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		// 3. Enable the TXEIE (Transmit Data Register Empty Interrupt Enable) bit
		// This tells the hardware to generate an interrupt whenever TXE becomes 1
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);

		// 4. Enable the TCIE (Transmission Complete Interrupt Enable) bit
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
	}

	return txstate;
}



/*********************************************************************
 * @fn      		- USART_ReceiveDataIT
 *
 * @brief           - Initiates a non-blocking reception using Interrupts
 *
 * @param[in]		- pUSARTHandle: Pointer to the USART handle structure
 * @param[in]		- pRxBuffer: Pointer to the memory where received data will be stored
 * @param[in]		- Len: Number of bytes to receive
 *
 * @return			- uint8_t: Returns the current state (USART_READY or USART_BUSY_IN_RX)
 *********************************************************************/
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t rxstate = pUSARTHandle->RxBusyState;

	if (rxstate != USART_BUSY_IN_RX)
	{
		// 1. Save the Rx buffer address and length information in the handle
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxLen = Len;

		// 2. Update the USART state to BUSY
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		// 3. Enable the RXNEIE (Receive Data Register Not Empty Interrupt Enable) bit
		// This tells the hardware to generate an interrupt whenever a new byte arrives
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
	}

	return rxstate;
}



/*********************************************************************
 * @fn      		- USART_IRQHandling
 *
 * @brief           - Handles the background data transfer when an interrupt is triggered
 *
 * @param[in]		- pUSARTHandle: Pointer to the USART handle structure
 *
 * @return			- none
 *********************************************************************/
void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{
	uint32_t temp1, temp2;
	uint16_t *pdata;

	// ==========================================================
	// 1. Handle TXE (Transmit Data Register Empty) Interrupt
	// ==========================================================
	// Check if the TXE flag is set in the ISR register
	temp1 = pUSARTHandle->pUSARTx->ISR & (1 << USART_ISR_TXE);
	// Check if the TXE interrupt was actually enabled in the CR1 register
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TXEIE);

	if (temp1 && temp2)
	{
		// We are here because the TDR is empty! Let's check if we actually have data to send
		if (pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			// Keep sending data as long as TxLen is greater than 0
			if (pUSARTHandle->TxLen > 0)
			{
				// Check the Word Length (M0 and M1 bits)
				uint8_t val_M0 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_M0) & 0x1;
				uint8_t val_M1 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_M1) & 0x1;

				if (val_M0 == 1 && val_M1 == 0)
				{
					// --- 9-BIT MODE ---
					pdata = (uint16_t *) pUSARTHandle->pTxBuffer;
					pUSARTHandle->pUSARTx->TDR = (*pdata & (uint16_t)0x01FF);

					if ((pUSARTHandle->pUSARTx->CR1 >> USART_CR1_PCE) & 0x1)
					{
						// Parity ON
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen--;
					}
					else
					{
						// Parity OFF
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen -= 2;
					}
				}
				else
				{
					// --- 8-BIT OR 7-BIT MODE ---
					pUSARTHandle->pUSARTx->TDR = (*(pUSARTHandle->pTxBuffer) & (uint8_t)0xFF);
					pUSARTHandle->pTxBuffer++;
					pUSARTHandle->TxLen--;
				}
			}

			// Check if that was the very last byte
			if (pUSARTHandle->TxLen == 0)
			{
				// We have loaded the final byte into the TDR.
				// Turn OFF the TXE doorbell so it doesn't trap the CPU in an endless loop
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TXEIE);
			}
		}
	}

	// ==========================================================
	// 2. Handle TC (Transmission Complete) Interrupt
	// ==========================================================
	temp1 = pUSARTHandle->pUSARTx->ISR & (1 << USART_ISR_TC);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TCIE);

	if (temp1 && temp2)
	{
		// We are here because the very last bit has physically left the pin
		if (pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			// 1. Turn OFF the TC doorbell
			pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE);

			// 2. Reset the application state so it can accept a new message later
			pUSARTHandle->TxBusyState = USART_READY;

			// 3. Reset the buffer pointers and length to be safe
			pUSARTHandle->pTxBuffer = NULL;
			pUSARTHandle->TxLen = 0;

			// 4. Call the application callback to let main.c know the transmission is 100% finished
			USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_TX_CMPLT /* USART_EVENT_TX_CMPLT */);
		}
	}

	// ==========================================================
	// 3. Handle RXNE (Receive Data Register Not Empty) Interrupt
	// ==========================================================
	temp1 = pUSARTHandle->pUSARTx->ISR & (1 << USART_ISR_RXNE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);

	if (temp1 && temp2)
	{
		// We are here because a new byte just arrived from the wire
		if (pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
		{
			if (pUSARTHandle->RxLen > 0)
			{
				uint8_t val_M0 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_M0) & 0x1;
				uint8_t val_M1 = (pUSARTHandle->pUSARTx->CR1 >> USART_CR1_M1) & 0x1;

				if (val_M0 == 1 && val_M1 == 0)
				{
					// --- 9-BIT MODE ---
					if ((pUSARTHandle->pUSARTx->CR1 >> USART_CR1_PCE) & 0x1)
					{
						// Parity ON
						// Apply the & 0xFF mask to wipe out any garbage and guarantee we only have 8 clean bits
						// We dereference so compiler says "Okay, go to that address, open the 8-bit box, and get ready to drop data inside"
						*((uint8_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->RDR & (uint8_t)0xFF);
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen--;
					}
					else
					{
						// Parity OFF
						*((uint16_t *) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->RDR & (uint16_t)0x01FF);
						pUSARTHandle->pRxBuffer += 2;
						pUSARTHandle->RxLen -= 2;
					}
				}
				else
				{
					// --- 8-BIT OR 7-BIT MODE ---
					if ((pUSARTHandle->pUSARTx->CR1 >> USART_CR1_PCE) & 0x1)
					{
						// Parity ON
						*((uint8_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->RDR & (uint8_t)0x7F);
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen--;
					}
					else
					{
						// Parity OFF
						*((uint8_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->RDR & (uint8_t)0xFF);
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen--;
					}
				}
			}

			// Check if we have received all the requested bytes
			if (pUSARTHandle->RxLen == 0)
			{
				// 1. Turn OFF the RXNE doorbell
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_RXNEIE);

				// 2. Reset the application state
				pUSARTHandle->RxBusyState = USART_READY;

				// 3. Call the application callback to tell main.c we got all the data
				USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_RX_CMPLT /* USART_EVENT_RX_CMPLT */);
			}
		}
	}
}



/*********************************************************************
 * @fn      		- USART_IRQInterruptConfig
 *
 * @brief           - Configures the NVIC (ARM Cortex Processor) to enable or disable interrupts
 *
 * @param[in]		- IRQNumber: The IRQ number of the USART (e.g., 37 for USART1, 38 for USART2)
 * @param[in]		- EnorDi: ENABLE or DISABLE
 *
 * @return			- none
 *********************************************************************/
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			// Program ISER0 register (IRQs 0 to 31)
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			// Program ISER1 register (IRQs 32 to 63)
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if (IRQNumber >= 64 && IRQNumber < 96)
		{
			// Program ISER2 register (IRQs 64 to 95)
			*NVIC_ISER2 |= (1 << (IRQNumber % 32));
		}
	}
	else
	{
		if (IRQNumber <= 31)
		{
			// Program ICER0 register (IRQs 0 to 31)
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			// Program ICER1 register (IRQs 32 to 63)
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if (IRQNumber >= 64 && IRQNumber < 96)
		{
			// Program ICER2 register (IRQs 64 to 95)
			*NVIC_ICER2 |= (1 << (IRQNumber % 32));
		}
	}
}



/*********************************************************************
 * @fn      		- USART_IRQPriorityConfig
 *
 * @brief           - Configures the priority of the given IRQ number in the NVIC
 *
 * @param[in]		- IRQNumber: The IRQ number
 * @param[in]		- IRQPriority: The priority value (0 to 15, lower number = higher priority)
 *
 * @return			- none
 *********************************************************************/
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. Find the corresponding IPR (Interrupt Priority Register)
	// Each register holds 4 priorities, so divide by 4 to find the correct register index
	uint8_t iprx = IRQNumber / 4;

	// 2. Find the exact 8-bit section inside that register
	uint8_t iprx_section = IRQNumber % 4;

	// 3. Calculate the shift amount
	// STM32 microcontroller only implements the upper 4 bits of each 8-bit section
	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	// 4. Apply the priority to the correct register
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}



/*********************************************************************
 * @fn      		- USART_ClearFlag
 *
 * @brief           - Clears a specific flag in the USART peripheral
 *
 * @param[in]		- pUSARTx: Base address of the USART peripheral
 * @param[in]		- StatusFlagName: The macro of the flag to clear
 *
 * @return			- none
 *********************************************************************/
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
	// Direct assignment is faster and safer for the ICR register.
	// Writing a 0 has no effect, so it won't accidentally clear other flags.
	pUSARTx->ICR = StatusFlagName;
}



/*********************************************************************
 * @fn      		- USART_InterruptControl
 *
 * @brief           - Enables or Disables specific USART hardware interrupts
 *
 * @param[in]		- pUSARTx: Pointer to the USART peripheral register definition structure
 * @param[in]		- interrupt_type: The specific interrupt bit to configure (e.g., USART_IT_RXNE)
 * @param[in]		- EnOrDi: ENABLE or DISABLE macro
 *
 * @return			- None
 *
 * @note			- Modifies the CR1 register to enable/disable hardware interrupt generation.
 * Ensure NVIC is also enabled in the application layer.
 *********************************************************************/
void USART_InterruptControl(USART_RegDef_t *pUSARTx, uint8_t interrupt_type, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		// Turn ON the specific interrupt bit
		pUSARTx->CR1 |= (1 << interrupt_type);
	}
	else
	{
		// Turn OFF the specific interrupt bit
		pUSARTx->CR1 &= ~(1 << interrupt_type);
	}
}



/*********************************************************************
 * @fn      		- USART_ApplicationEventCallback
 *
 * @brief           - Application callback function for USART events
 *
 * @param[in]		- pUSARTHandle: Pointer to the USART handle structure
 * @param[in]		- AppEv: The event macro that triggered the callback
 *
 * @return			- none
 * * @note          - This is a __weak function. The application should
 * override this to implement custom event handling.
 *********************************************************************/
__weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t AppEv)
{
	// This is a weak implementation. The application (main.c) may override this.
	// If the user doesn't write their own version in main.c, this empty one runs safely.
}
