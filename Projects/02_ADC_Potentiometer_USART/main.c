#include "stm32l47xxx.h"
#include <stdio.h>
#include <string.h>

// Global variables
ADC_Handle_t adc1_handle;
USART_Handle_t usart1_handle;

__vo uint16_t adc_value = 0;		// Holds the 0-1023 reading
__vo uint8_t alarm_active = 0;	   	// Flag for the blinking LED
__vo uint32_t system_ms_ticks = 0; 	// Our master clock

char msg_buffer[50];	// Holds the text before sending to PC

// ---------------------------------------------------------
// 4. Hardware Initialization Functions
// ---------------------------------------------------------
void GPIO_Setup(void)
{
	GPIO_Handle_t gpio_leds, gpio_analog;

	// Enable Clocks for GPIOA (Analog pin) and GPIOC (LED pins)
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOC, ENABLE);

	// --- Potentiometer Pin (PA0 = ADC1_IN5) ---
	gpio_analog.pGPIOx = GPIOA;
	gpio_analog.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	gpio_analog.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	gpio_analog.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&gpio_analog);

	// --- LED Pins (PC0: Green, PC1: Yellow, PC2: Red) ---
	gpio_leds.pGPIOx = GPIOC;
	gpio_leds.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	gpio_leds.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	gpio_leds.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	gpio_leds.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;

	gpio_leds.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0; // Green
	GPIO_Init(&gpio_leds);
	gpio_leds.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1; // Yellow
	GPIO_Init(&gpio_leds);
	gpio_leds.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2; // Red
	GPIO_Init(&gpio_leds);
}

void USART1_Setup(void)
{
	// 1. Turn on the USART hardware clock
	USART_PeriClockControl(USART1, ENABLE);

	// 2. USART PA9/PA10 GPIO Init
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Handle_t usart_gpios;

	usart_gpios.pGPIOx = GPIOA;
	usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

	// USART TX on PA9, RX on PA10
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&usart_gpios);
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIO_Init(&usart_gpios);

	// USART Init
	usart1_handle.pUSARTx = USART1;
	usart1_handle.USART_Config.USART_Baud = 9600;
	usart1_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart1_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart1_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart1_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart1_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart1_handle.USART_Config.USART_Oversampling = USART_OVERSAMPLING_16;

	USART_Init(&usart1_handle);
	USART_PeripheralControl(USART1, ENABLE);
}

void ADC1_Setup(void)
{
	adc1_handle.pADCx = ADC1;
	adc1_handle.ADC_Config.ADC_Channel = ADC_CH_5;	// PA0
	adc1_handle.ADC_Config.ADC_Resolution = ADC_RES_10_BIT;	// 0 to 1023
	adc1_handle.ADC_Config.ADC_SampleTime = ADC_SAMPLE_24_5_CYCLES;

	ADC_Init(&adc1_handle);
}

void SysTick_Setup(void)
{
	// Configure hardware timer to interrupt every 1 millisecond
	// (16,000 ticks at 16MHz)
	SysTick_Init(16000);
}

// ---------------------------------------------------------
// 5. Application Logic
// ---------------------------------------------------------
void Update_LEDs(uint32_t val)
{
	if (val < 340)
	{
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_0, GPIO_PIN_SET);	// Green
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_1, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_2, GPIO_PIN_RESET);
		alarm_active = 0;
	}
	else if (val >= 340 && val < 680)
	{
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_0, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_1, GPIO_PIN_SET);	// Yellow
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_2, GPIO_PIN_RESET);
		alarm_active = 0;
	}
	else if (val >= 680 && val < 900)
	{
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_0, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_1, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_2, GPIO_PIN_SET); // Red solid
		alarm_active = 0;
	}
	else if (val >= 900)
	{
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_0, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOC, GPIO_PIN_NO_1, GPIO_PIN_RESET);
		alarm_active = 1; // Let SysTick blink the Red LED
	}
}

// ---------------------------------------------------------
// 6. Interrupt Service Routines (The Doorbells)
// ---------------------------------------------------------
// DOORBELL 1: The ADC finished reading!
void ADC1_2_IRQHandler(void)
{
	// Ask the driver: "Did the EOC flag trigger this?"
	if (ADC_GetFlagStatus(ADC1, ADC_ISR_EOC_POS))
	{
		// 1. Ask the driver for the data
		adc_value = ADC_ReadDataReg(ADC1);

		// 2. Process the LED logic instantly
		Update_LEDs(adc_value);
	}
}

// DOORBELL 2: The Timer ticked! (Happens every 1 millisecond)
void SysTick_Handler(void)
{
	// 1. Advance the master clock by 1 millisecond
	system_ms_ticks++;

	// 2. Check if we need to blink the alarm
	if (alarm_active == 1)
	{
		// Is the current millisecond evenly divisible by 500?
		if(system_ms_ticks % 500 == 0)
		{
			GPIO_ToggleOutputPin(GPIOC, GPIO_PIN_NO_2); // Toggle Red LED
		}
	}
}

int main(void)
{
	// 1. System Configuration
	RCC_SystemClock_ConfigTo16MHz();

	// 2. Init all hardware
	GPIO_Setup();
	USART1_Setup();
	ADC1_Setup();
	SysTick_Setup();

	// 3. Enable the ADC Interrupt in the CPU's brain (NVIC)
	// IRQ 18 is the specific Doorbell line for ADC1 on the STM32L476
	ADC_IRQInterruptConfig(IRQ_NO_ADC1_2, ENABLE);

	// 4. Kick off the very first background reading
	ADC_StartConversion_IT(ADC1);

	// Local variable to track when we last printed text
	uint32_t last_print_time = 0;

	// 5. The 100% Non-Blocking Super Loop
	while (1)
	{
		// Check the Master Clock: Have 100 milliseconds passed?
		if ((system_ms_ticks - last_print_time) >= 250)
		{
			// YES! Time to print. Update the timestamp first
			last_print_time = system_ms_ticks;

			// Format and send the potentiometer text
			sprintf(msg_buffer, "Pot Value: %u\r\n", adc_value);
			USART_SendData(USART1, (uint8_t*)msg_buffer, strlen(msg_buffer));

			// Tell the ADC to take another reading in the background
			ADC_StartConversion_IT(ADC1);
		}
	}

	return 0;
}


