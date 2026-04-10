#include "stm32l47xxx_gpio_driver.h"


/*
 * Peripheral Clock setup
 */

/********************************************************
 * @fn				- GPIO_PeriClockControl
 *
 * @brief			- This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]		- base address of the gpio peripheral
 * @param[in]		- ENABLE or DISABLE macros
 *
 * @return			none
 *
 * @Note			none
 */

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		} else if (pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		} else if (pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		} else if (pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		} else if (pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		} else if (pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();
		} else if (pGPIOx == GPIOG)
		{
			GPIOG_PCLK_EN();
		} else if (pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();
		}
	} else
	{
		if (pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		} else if (pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();
		} else if (pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		} else if (pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		} else if (pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		} else if (pGPIOx == GPIOF)
		{
			GPIOF_PCLK_DI();
		} else if (pGPIOx == GPIOG)
		{
			GPIOG_PCLK_DI();
		} else if (pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();
		}
	}
}



/*
 * Init and De-init
 *
 * GPIO_Init() needs:
 * Which port? → pGPIOx
 * Which pin & how to configure it? → GPIO_PinConfig
 *
 * De-init usually means:
 * Reset the entire GPIO port using RCC reset register
 * Only the port base address is needed
 */

/********************************************************
 * @fn				- GPIO_Init
 *
 * @brief			- Initializes the GPIO pin according to the
 * 					  configuration specified in the GPIO handle
 *
 * @param[in]		- pGPIOHandle : Pointer to GPIO_Handle_t structure
 * 					  that contains GPIO base address and pin configuration
 *
 * @return			- none
 *
 * @Note			- This function configures GPIO pin mode, speed,
 * 					  pull-up/pull-down, output type, and alternate
 * 					  function if applicable. (8.5 in RM)
 */

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0;	// temp register
	// 1. Configure the mode of the gpio pin
	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		// The non interrupt mode
		temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));	// clearing
		pGPIOHandle->pGPIOx->MODER |= temp;	// setting
		/*
		 * GPIO_PinMode   = GPIO_MODE_OUTPUT   // = 1 → 01
		 * GPIO_PinNumber = 5
		 * 1 << (2 * 5)
		 * 1 << 10
		 *
		 * Binary result:
		 * 0000 0000 0000 0000 0000 0100 0000 0000
                 	 	 	 	 	 ↑
               	   	   	   	   	   MODE5 = 01
         * |= -> All other MODER bits remain unchanged (bitwise or)
         *
         * 0x3 is 11 in binary and we negate it so we have 00
         * Example for pin 5:
         * 0x3 << 10
         * Before ~ :
         * 0000 0000 0000 0000 0000 1100 0000 0000
         * After  ~ :
         * 1111 1111 1111 1111 1111 0011 1111 1111
         * Pin 5 cleared
		 */
	} else
	{
		// Interrupt mode
		if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			// 1. Configure the FTSR1
			EXTI->FTSR1 |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding RTSR1 bit
			EXTI->RTSR1 &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			// 1. Configure the RTSR1
			EXTI->RTSR1 |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding FTSR1 bit
			EXTI->FTSR1 &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			// 1. Configure both FTSR and RTSR
		    EXTI->RTSR1 |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		    EXTI->FTSR1 |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		// 2. Configure the GPIO port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] &= ~(0xF << (temp2 * 4));
		SYSCFG->EXTICR[temp1] |= (portcode << (temp2 * 4));

		// 3. Enable the exti interrupt delivery using IMR1
		EXTI->IMR1 |= 1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
		// “Hey, I want interrupts from this pin to be allowed to go to NVIC.”

	}

	temp = 0;

	// 2. Configure the speed
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));	// clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;

	// 3. Configure the pupd settings
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));	// clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	// 4. Configure the optype
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);	// clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	temp = 0;

	// 5. Configure the alt functionality
	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		// Configure the alt fun
		uint8_t temp1, temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));	// clearing
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
		/*
		 * temp2 = 5
		 * shift = 4 * 5 = 20
		 * AF7:
		 * 7 << 20
		 * Binary:
		 * 0000 0000 0111 0000 0000 0000 0000 0000
              	  	 ↑↑↑↑
            		AF5 = 0111
		 */
	}

	// ==========================================================
	// 6. STM32L4 QUIRK: CLOSE THE ANALOG SWITCH
	// ==========================================================
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ANALOG)
	{
		// Close the physical switch connecting the pin to the ADC multiplexer
		pGPIOHandle->pGPIOx->ASCR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	else
	{
		// If it's not analog, make sure the switch is open to save power
		pGPIOHandle->pGPIOx->ASCR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
}



/********************************************************
 * @fn				- GPIO_DeInit
 *
 * @brief			- Resets the GPIO peripheral registers to their
 * 					  default reset values
 *
 * @param[in]		- pGPIOx : Base address of the GPIO peripheral
 *
 * @return			- none
 *
 * @Note			- This function resets all registers of the given
 * 					  GPIO port using the RCC reset control
 */

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if (pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	} else if (pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();
	} else if (pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();
	} else if (pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();
	} else if (pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();
	} else if (pGPIOx == GPIOF)
	{
		GPIOF_REG_RESET();
	} else if (pGPIOx == GPIOG)
	{
		GPIOG_REG_RESET();
	} else if (pGPIOx == GPIOH)
	{
		GPIOH_REG_RESET();
	}
}



/*
 * Data read and write
 *
 * GPIO_ReadFromInputPin(): the result is 0 or 1 (reads 1 bit)
 * That's why return type is uint8_t
 *
 * GPIO_ReadFromInputPort(): we want the entire IDR register (all 16 pins at once)
 * GPIO port has 16 pins that's why uint16_t is used (reads 16 bits)
 */

/********************************************************
 * @fn				- GPIO_ReadFromInputPin
 *
 * @brief			- Reads the logic level present on a specific
 *					  GPIO input pin
 *
 * @param[in]		- pGPIOx : Base address of the GPIO peripheral
 * @param[in]		- PinNumber : GPIO pin number to be read
 *
 * @return			- uint8_t : Logic level of the pin (0 or 1)
 *
 * @Note			- This function reads the value from the input
 *					  data register (IDR) of the GPIO port.
 *					  Perfect for “is the pin high or low”
 * Suppose PinNumber = 5:
 * IDR = 0b00000000 00100000
 * Shift >> 5:
 * 0b00000000 00000001	(pin we care about is in least significant bit (LSB))
 *
 * & 0x01 ensures all other bits are cleared
 * Only LSB remains
 * So the value is either 0 or 1
 */

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;

	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);

	return value;
}



/********************************************************
 * @fn				- GPIO_ReadFromInputPort
 *
 * @brief			- Reads the logic levels present on all pins of
 *					  a GPIO input port
 *
 * @param[in]		- pGPIOx : Base address of the GPIO peripheral
 *
 * @return			- uint16_t : Value of the GPIO input data register
 *
 * @Note			- This function reads the complete input data
 *					  register (IDR) of the GPIO port
 *
 * 16 bits are used for pins
 * Therefore, if you read all pins at once, you need 16 bits, not 8 (uint16_t)
 * If we use uint8_t (8 bits), we can only see pins 7–0
 * Pins 8–15 are lost
 *
 * IDR as 16-bit value
 * Binary: 0001 0000 1001 0101
 * Convert to hex: 0x1095
 * Convert to decimal: 4245
 * Returns: uint16_t port_val = 4245;  // decimal
 *
 * We check pin 4:
 * if (port_val & (1 << 4))
 *    0001 0000 1001 0101
 *  & 0000 0000 0001 0000
 *    -----------------------
 *    0000 0000 0001 0000
 *
 * Result = 16 → pin 4 is high (every positive value is true)
 *
 * Using hex makes it easy to see each pin’s bit
 * Binary: 0001 0000 1001 0101  → Hex: 0x1095 → Decimal: 4245
 * Hex shows exactly which bits are 1 without converting manually
 */

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;

	value = (uint16_t)pGPIOx->IDR;

	return value;
}



/********************************************************
 * @fn				- GPIO_WriteToOutputPin
 *
 * @brief			- Writes a logic value to a specific GPIO output pin
 *
 * @param[in]		- pGPIOx : Base address of the GPIO peripheral
 * @param[in]		- PinNumber : GPIO pin number to write to
 * @param[in]		- Value : Value to be written to the pin
 *					  (0 = LOW, 1 = HIGH)
 *
 * @return			- none
 *
 * @Note			- This function modifies only the specified pin
 *					  in the output data register (ODR) without
 *					  affecting other pins
 *
 * The GPIO output driver drives the pin to logic HIGH
 * That usually means:
 * ~3.3V (or 5V, depending on MCU)
 * The pin is actively sourcing current
 * Electrical result:
 * The pin voltage goes HIGH
 * Anything connected to that pin sees a HIGH level
 *
 * The GPIO output driver drives the pin to logic LOW
 *
 * The pin is connected to GND internally
 * Electrical result:
 * The pin voltage goes LOW (≈ 0V)
 * Current can flow into the pin from external circuitry
 */

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if (Value == GPIO_PIN_SET)
	{
		// Write 1 to the output data register at the bit field corresponding to the pin number
		pGPIOx->ODR |= (1 << PinNumber);
	} else
	{
		// Write 0
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}



/********************************************************
 * @fn				- GPIO_WriteToOutputPort
 *
 * @brief			- Writes a value to the entire GPIO output port
 *
 * @param[in]		- pGPIOx : Base address of the GPIO peripheral
 * @param[in]		- Value : Value to be written to the GPIO output
 *					  data register
 *
 * @return			- none
 *
 * @Note			 This function overwrites the entire output data
 *					  register (ODR); all GPIO pins are updated
 *					  simultaneously
 *
 * GPIO_WriteToOutputPort(GPIOB, 0x3F); (Example)
 * 0x3F = 3 * 16 + F = 48 + 15 = 63 (decimal)
 * 0x3F = 0011 1111 (binary)
 * Bit:  7 6 5 4 3 2 1 0
      	 0 0 1 1 1 1 1 1	(Here you see which pin is high)
 *
 *
 */

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint8_t Value)
{
	pGPIOx->ODR = Value;	/* We write to whole port so we just copy the value to ODR register */
}



/********************************************************
 * @fn				- GPIO_ToggleOutputPin
 *
 * @brief			- Toggles the current logic level of a specific
 *					  GPIO output pin
 *
 * @param[in]		- pGPIOx : Base address of the GPIO peripheral
 * @param[in]		- PinNumber : GPIO pin number to be toggled
 *
 * @return			- none
 *
 * @Note			- This function inverts the bit corresponding to
 *					  the selected pin in the output data register
 *					  (ODR) without affecting other pins
 *
 * Example:
 * ODR = 0010 1101   (binary) (Pin 0 = 1, Pin 2 = 1 etc.)
 * Toggle Pin 3: Mask = 0000 1000
 *    ODR     = 0010 1101
 *   ^ Mask   = 0000 1000
 *   -------------------
 *   Result   = 0010 0101
 *
 * Bit 3: 1 → 0
 * All other bits: unchanged
 */

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}



/*
 * IRQ Configuration and ISR handling
 *
 * IRQ = Interrupt Request
 * An IRQ is a signal sent to the CPU saying:
 * “STOP what you’re doing, something important just happened!”
 * Purpose: Configure the NVIC, not the GPIO itself (en/di interrupt line, set priority)
 *
 * IRQNumber: Which interrupt line in NVIC
 * IRQPriority: Interrupt urgency level (Lower number = higher priority)
 *
 * GPIO_IRQHandling(): Clear the EXTI pending flag for that pin
 * EXTI lines are pin-based (EXTI0 ↔ Pin 0, etc.)
 * Port doesn’t matter here
 * Clearing interrupt = clearing the EXTI line
 */

/********************************************************
 * @fn				- GPIO_IRQInterruptConfig
 *
 * @brief			- Configures the IRQ number
 *
 * @param[in]		- IRQNumber : IRQ number of the GPIO interrupt
 * @param[in]		- EnorDi : ENABLE or DISABLE the IRQ
 *
 * @return			- none
 *
 * @Note			- This function configures the NVIC interrupt
 *					  enable/disable
 */

void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			// ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);

		} else if (IRQNumber > 31 && IRQNumber < 64)	// 32 to 63
		{
			// ISER1 register
			*NVIC_ISER1 |= (1 << IRQNumber % 32);

		} else if (IRQNumber >= 64 && IRQNumber < 96)	// 64 to 95
		{
			// ISER2 register
			*NVIC_ISER2 |= (1 << IRQNumber % 32);
		}
	} else
	{
		if (IRQNumber <= 31)
		{
			// ISER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);

		} else if (IRQNumber > 31 && IRQNumber < 64)	// 32 to 63
		{
			// ISER1 register
			*NVIC_ICER1 |= (1 << IRQNumber % 32);

		} else if (IRQNumber >= 64 && IRQNumber < 96)	// 64 to 95
		{
			// ISER2 register
			*NVIC_ICER2 |= (1 << IRQNumber % 32);
		}
	}
}



/*********************************************************************
 * @fn      		  - GPIO_IRQPriorityConfig
 *
 * @brief             - Configures the priority level for a specific IRQ number
 * 						by calculating the correct Priority Register (IPR)
 * 						and bit offset within that register.
 *
 * @param[in]         - IRQNumber: The position of the interrupt in the vector table.
 * @param[in]         - IRQPriority: The priority value to be assigned (0-255).
 *
 * @return            - none
 *
 * @Note              - In ARM Cortex-M, only the top bits of the priority byte
 * 						are usually implemented. This function accounts for
 * 						the non-implemented lower bits using NO_PR_BITS_IMPLEMENTED.
 */

void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    volatile uint32_t *ipr = NVIC_PR_BASE_ADDR + iprx;

    /* Clear the existing priority for this IRQ's section (one byte) */
    *ipr &= ~((uint32_t)0xFFU << (8U * iprx_section));

    /* Set the new priority value, respecting NO_PR_BITS_IMPLEMENTED */
    *ipr |= ((uint32_t)IRQPriority << shift_amount);
}


/********************************************************
 * @fn				- GPIO_IRQHandling
 *
 * @brief			- Handles an interrupt triggered by a specific
 *					  GPIO pin
 *
 * @param[in]		- PinNumber : GPIO pin number that caused the IRQ
 *
 * @return			- none
 *
 * @Note			- This function clears the pending interrupt
 *					  bit in the EXTI pending register (PR) for
 *					  the selected pin to prevent repeated IRQs
 *
 * To clear a pending interrupt, you don't write a 0; you must write a 1 to the bit
 *
 * If you forget to clear this bit, the CPU will exit
 * the IRQ handler and immediately jump back into it forever,
 * because the hardware still thinks an interrupt is pending
 */

void GPIO_IRQHandling(uint8_t PinNumber)
{
	// Clear the exti pr register corresponding to the pin number
	if (EXTI->PR1 & (1 << PinNumber))
	{
		// clear
		EXTI->PR1 |= (1 << PinNumber);
	}
}
