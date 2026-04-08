#ifndef STM32L47XXX_I2C_DRIVER_H
#define STM32L47XXX_I2C_DRIVER_H

#include "stm32l47xxx.h"

/*
 * Configuration structure for I2Cx peripheral
 */
typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t	 I2C_DeviceAddress;
	uint8_t  I2C_AckControl;
}I2C_Config_t;

/*
 * Handle structure for I2Cx peripheral
 */
typedef struct
{
	I2C_RegDef_t 	*pI2Cx;
	I2C_Config_t 	I2C_Config;
	uint8_t 		*pTxBuffer;		/* Store the app Tx buffer */
	uint8_t 		*pRxBuffer;		/* Store the app Rx buffer */
	uint32_t 		TxLen;			/* Store Tx Len */
	uint32_t 		RxLen;			/* Store Rx Len */
	uint8_t 		TxRxState;		/* Store Communication state */
	uint8_t 		DevAddr;		/* Store slave/device address */
	uint32_t 		RxSize;			/* Store Rx size */
	uint8_t 		Sr;				/* Store repeated start value */
} I2C_Handle_t;

/*
 * I2C application states
 */
#define I2C_READY			0
#define I2C_BUSY_IN_RX		1
#define I2C_BUSY_IN_TX		2

/*
 * @I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM		100000		/* Standard Mode 100 kHz */
#define I2C_SCL_SPEED_FM4K		400000		/* Fast Mode 400 kHz */
#define I2C_SCL_SPEED_FM2K		200000		/* Fast Mode 200 kHz */
#define I2C_SCL_SPEED_FM_PLUS   1000000   	/* Fast Mode Plus 1 MHz */

/*
 * @I2C_AckControl
 */
#define I2C_ACK_ENABLE			1
#define I2C_ACK_DISABLE			0

/*
 * I2C related status flags definitions (from ISR register)
 */
#define I2C_FLAG_TXE        (1 << I2C_ISR_TXE)
#define I2C_FLAG_TXIS       (1 << I2C_ISR_TXIS)
#define I2C_FLAG_RXNE       (1 << I2C_ISR_RXNE)
#define I2C_FLAG_TC         (1 << I2C_ISR_TC)
#define I2C_FLAG_TCR        (1 << I2C_ISR_TCR)
#define I2C_FLAG_STOPF      (1 << I2C_ISR_STOPF)
#define I2C_FLAG_BUSY       (1 << I2C_ISR_BUSY)
#define I2C_FLAG_NACKF      (1 << I2C_ISR_NACKF)
#define I2C_FLAG_DIR        (1 << I2C_ISR_DIR)

/*
 * I2C Application Event Macros
 */
#define I2C_EV_TX_CMPLT  	0   // Transmit Complete
#define I2C_EV_RX_CMPLT  	1   // Receive Complete
#define I2C_EV_STOP       	2   // Stop condition generated
#define I2C_ERROR_BERR 		3   // Bus Error
#define I2C_ERROR_ARLO  	4   // Arbitration Loss Error
#define I2C_ERROR_AF    	5   // Acknowledge Failure
#define I2C_ERROR_OVR   	6   // Overrun/Underrun Error
#define I2C_ERROR_TIMEOUT 	7   // Timeout Error



/*********************************************************************
 * 						APIs supported by this driver
 *********************************************************************/
/*
 * Peripheral Clock setup
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);

/*
 * Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr);
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr);

void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C);

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/*
 * IQR Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

/*
 * Application state management APIs
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);

/*
 * Other Peripheral Control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);


/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);


#endif /* STM32L47XXX_I2C_DRIVER_H */
