#include "stm32l47xxx.h"
#include <stdint.h>

#define MPU6050_ADDR	0x68
#define WHO_AM_I_REG    0x75

// Global variables
I2C_Handle_t i2c1_handle;
USART_Handle_t usart1_handle;

// Buffers
uint8_t reg_to_read = WHO_AM_I_REG;
uint8_t rx_data;	// This will hold the MPU6050's answer

/*****************************************
 * 1. GPIO & Peripheral Initialization
 *****************************************/
void I2C1_GPIOInit(void)
{
	GPIO_Handle_t i2c_pins;
	GPIO_PeriClockControl(GPIOB, ENABLE);

	i2c_pins.pGPIOx = GPIOB;
	i2c_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_pins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;		// AF4 for I2C1
	i2c_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD; // CRITICAL: Open Drain for I2C!
	i2c_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	i2c_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// PB6 = SCL
	i2c_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&i2c_pins);

	// PB7 = SDA
	i2c_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&i2c_pins);
}

void I2C1_Init(void)
{
	i2c1_handle.pI2Cx = I2C1;
	i2c1_handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	i2c1_handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&i2c1_handle);
}

void USART1_GPIOInit(void)
{
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
}

void USART1_Init(void)
{
	usart1_handle.pUSARTx = USART1;
	usart1_handle.USART_Config.USART_Baud = 9600;
	usart1_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart1_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart1_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart1_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart1_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart1_handle.USART_Config.USART_Oversampling = USART_OVERSAMPLING_16;

	USART_Init(&usart1_handle);
}

/**************************************
 * 2. The main routine
 **************************************/
int main(void)
{
	RCC_SystemClock_ConfigTo16MHz();

	// Initialize hardware
	USART1_GPIOInit();
	USART1_Init();
	USART_PeripheralControl(USART1, ENABLE);

	I2C1_GPIOInit();
	I2C1_Init();
	I2C_PeripheralControl(I2C1, ENABLE);

	// Enable I2C Interrupts in the NVIC (IRQ 31 is Events, IRQ 32 is Errors)
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	// Enable USART Interrupts so SendDataIT actually works
	USART_IRQInterruptConfig(IRQ_NO_USART1, ENABLE);

	// A few milliseconds for the MPU6050 to power up and stabilize
	for (uint32_t i = 0; i < 50000; i++);

	// KICK OFF THE PROCESS:
	// We initiate a 1-byte WRITE to send the register address (0x75) we want to look at.
	// This function returns instantly. The hardware takes over in the background!
	I2C_MasterSendDataIT(&i2c1_handle, &reg_to_read, 1, MPU6050_ADDR);

	// Foreground Task does nothing while I2C works in the background!
	while (1) {}	// we need to keep CPU alive

	return 0;
}

/*******************************************
 * 3. The Hardware Doorbell Handlers (ISRs)
 ******************************************/
void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&i2c1_handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&i2c1_handle);
}

void USART1_IRQHandler(void)
{
	USART_IRQHandling(&usart1_handle);
}

/****************************************************
 * 4. The Application Callback (The State Machine)
 ****************************************************/
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
	// STEP 1: The transmission of the register address (0x75) finished successfully
	if (AppEv == I2C_EV_TX_CMPLT)
	{
		// Now that the MPU6050's internal pointer is at 0x75, we immediately
		// request to READ 1 byte from it
		I2C_MasterReceiveDataIT(pI2CHandle, &rx_data, 1, MPU6050_ADDR);
	}

	// STEP 2: The reception finished successfully
	else if (AppEv == I2C_EV_RX_CMPLT)
	{
		// The MPU6050 answered us, and the data is sitting safely in 'rx_data'.
		// Send the hex byte over USART so we can see it on the Arduino Serial Monitor
		USART_SendDataIT(&usart1_handle, &rx_data, 1);
	}

	// STEP 3: Handle any potential wire issues cleanly
	else if (AppEv == I2C_ERROR_AF)
	{
		// Acknowledge Failure means the MPU6050 didn't respond to its address.
		// Check your wiring (SDA/SCL swapped?) or check if it has power
		I2C_CloseSendData(pI2CHandle);
	}
}


