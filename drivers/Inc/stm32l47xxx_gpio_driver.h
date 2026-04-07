#ifndef INC_STM32L47XXX_GPIO_DRIVER_H_
#define INC_STM32L47XXX_GPIO_DRIVER_H_

// This file contains driver specific data

#include "stm32l47xxx.h"



/*
 * This is a Configuration structure for a GPIO pin
 * They are stored by value, so GPIO_PinConfig doesn't have to be pointer
 */

typedef struct {
	uint8_t GPIO_PinNumber;			/* possible values from @GPIO_PIN_NUMBERS */
	uint8_t GPIO_PinMode;			/* possible values from @GPIO_PIN_MODES */
	uint8_t GPIO_PinSpeed;			/* possible values from @GPIO_PIN_SPEEDS */
	uint8_t GPIO_PinPuPdControl;	/* possible values from @GPIO_PIN_PUPDCONTROL */
	uint8_t GPIO_PinOPType;			/* possible values from @GPIO_PIN_OPTYPES */
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;



/*
 * This is a Handle structure for a GPIO pin
 * Think of GPIO_Handle_t as a “package” that fully describes ONE GPIO pin:
 * Where is the GPIO port?
 * How should the pin behave?
 * pGPIOx - WHERE the GPIO registers live
 * GPIO_PinConfig - HOW the pin should be configured
 * Pointer to struct cause GPIO_RegDef_t represents the GPIO hardware registers
 * That struct is a software map of the register layout defined in the reference manual
 */

typedef struct {
	GPIO_RegDef_t *pGPIOx;				/* This holds the base address of the GPIO port to which the pin belongs */
	GPIO_PinConfig_t GPIO_PinConfig;	/* This holds GPIO pin configuration settings */
}GPIO_Handle_t;



/*
 * @GPIO_PIN_NUMBERS
 * GPIO pin numbers
 */
#define GPIO_PIN_NO_0	0
#define GPIO_PIN_NO_1	1
#define GPIO_PIN_NO_2	2
#define GPIO_PIN_NO_3	3
#define GPIO_PIN_NO_4	4
#define GPIO_PIN_NO_5	5
#define GPIO_PIN_NO_6	6
#define GPIO_PIN_NO_7	7
#define GPIO_PIN_NO_8	8
#define GPIO_PIN_NO_9	9
#define GPIO_PIN_NO_10	10
#define GPIO_PIN_NO_11	11
#define GPIO_PIN_NO_12	12
#define GPIO_PIN_NO_13	13
#define GPIO_PIN_NO_14	14
#define GPIO_PIN_NO_15	15



/*
 * @GPIO_PIN_MODES
 * GPIO pin possible modes
 */
#define GPIO_MODE_IN			0
#define GPIO_MODE_OUT			1
#define GPIO_MODE_ALTFN			2
#define GPIO_MODE_ANALOG		3
#define GPIO_MODE_IT_FT			4		/* IT - input, FT - falling edge */
#define GPIO_MODE_IT_RT			5		/* RT - rising edge */
#define GPIO_MODE_IT_RFT		6		/* RTF - rising edge falling edge*/



/*
 * @GPIO_PIN_OPTYPES
 * GPIO pin possible output types
 */
#define GPIO_OP_TYPE_PP		0
#define GPIO_OP_TYPE_OD		1



/*
 * @GPIO_PIN_SPEEDS
 * GPIO pin possible output speeds
 */
#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_FAST			2
#define GPIO_SPEED_HIGH			3



/*
 * @GPIO_PIN_PUPDCONTROL
 * GPIO pin possible pupd configuration macros
 */
#define GPIO_NO_PUPD		0
#define GPIO_PIN_PU			1
#define GPIO_PIN_PD			2



/**********************************************************
 * 				APIs supported by this driver
 **********************************************************/

/*
 * Peripheral Clock setup
 */

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);



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

void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);



/*
 * Data read and write
 *
 * GPIO_ReadFromInputPin(): the result is 0 or 1 (reads 1 bit)
 * That's why return type is uint8_t
 *
 * GPIO_ReadFromInputPort(): we want the entire IDR register (all 16 pins at once)
 * GPIO port has 16 pins that's why uint16_t is used (reads 16 bits)
 */

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint8_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);



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

void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);




#endif /* INC_STM32L47XXX_GPIO_DRIVER_H_ */
