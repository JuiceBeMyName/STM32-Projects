#ifndef STM32L47XXX_USART_DRIVER_H_
#define STM32L47XXX_USART_DRIVER_H_

#include "stm32l47xxx.h"
#include "stm32l47xxx_rcc_driver.h"

/*
 * Configuration structure for USART peripheral
 */
typedef struct
{
	uint8_t		USART_Mode;
	uint32_t 	USART_Baud;
	uint8_t		USART_NoOfStopBits;
	uint8_t		USART_WordLength;
	uint8_t		USART_ParityControl;
	uint8_t		USART_HWFlowControl;
	uint8_t  	USART_Oversampling;
}USART_Config_t;

/*
 * Handle structure for USART peripheral
 */
typedef struct
{
	USART_RegDef_t *pUSARTx;		/* Pointer to the base address */
	USART_Config_t USART_Config;	/* USART configuration settings */
	uint8_t *pTxBuffer;				/* Pointer to the Tx buffer */
	uint8_t *pRxBuffer;				/* Pointer to the Rx buffer */
	uint8_t TxLen;					/* Tx Transfer length */
	uint8_t RxLen;					/* Rx Transfer length */
	uint8_t TxBusyState;			/* Tx Communication state */
	uint8_t RxBusyState;			/* Rx Communication state */
}USART_Handle_t;

/*
 * @USART_Mode
 */
#define USART_MODE_ONLY_TX		0
#define USART_MODE_ONLY_RX		1
#define USART_MODE_TXRX			2

/*
 * @USART_Baud
 */
#define USART_STD_BAUD_1200			1200
#define USART_STD_BAUD_2400			4200
#define USART_STD_BAUD_9600			9600
#define USART_STD_BAUD_19200 		19200
#define USART_STD_BAUD_38400 		38400
#define USART_STD_BAUD_57600 		57600
#define USART_STD_BAUD_115200 		115200
#define USART_STD_BAUD_230400 		230400
#define USART_STD_BAUD_460800 		460800
#define USART_STD_BAUD_921600 		921600
#define USART_STD_BAUD_2M 			2000000
#define USART_STD_BAUD_3_0M 		3000000		/* 3.0 Megabaud */

/*
 * @USART_WordLength
 */
#define USART_WORDLEN_8BITS  0
#define USART_WORDLEN_9BITS  1
#define USART_WORDLEN_7BITS  2

/*
 * @USART_NoOfStopBits
 */
#define USART_STOPBITS_1     0
#define USART_STOPBITS_0_5   1
#define USART_STOPBITS_2     2
#define USART_STOPBITS_1_5   3

/*
 * @USART_HWFlowControl
 */
#define USART_HW_FLOW_CTRL_NONE    	0
#define USART_HW_FLOW_CTRL_CTS    	1
#define USART_HW_FLOW_CTRL_RTS    	2
#define USART_HW_FLOW_CTRL_CTS_RTS	3

/*
 * @USART_Oversampling
 */
#define USART_OVERSAMPLING_16	0
#define USART_OVERSAMPLING_8	1

/*
 * USART Application States
 */
#define USART_READY 		0
#define USART_BUSY_IN_RX 	1
#define USART_BUSY_IN_TX 	2

/*
 * USART Application Event Macros
 */
#define USART_EVENT_TX_CMPLT   0   // Transmission Complete
#define USART_EVENT_RX_CMPLT   1   // Reception Complete
#define USART_EVENT_IDLE       2   // Idle line detected
#define USART_EVENT_CTS        3   // CTS flag
#define USART_EVENT_PE         4   // Parity Error
#define USART_ERR_FE           5   // Framing Error
#define USART_ERR_NE           6   // Noise Error
#define USART_ERR_ORE          7   // Overrun Error

/*
 * @USART_ParityControl
 * Possible options for USART parity
 */
#define USART_PARITY_DISABLE	0
#define USART_PARITY_EN_EVEN	1
#define USART_PARITY_EN_ODD		2

/*
 * USART related status flag definitions
 */
#define USART_FLAG_TXE 			(1 << USART_ISR_TXE)
#define USART_FLAG_RXNE 		(1 << USART_ISR_RXNE)
#define USART_FLAG_TC 			(1 << USART_ISR_TC)
#define USART_FLAG_ORE 			(1 << USART_ISR_ORE)
#define USART_FLAG_IDLE 		(1 << USART_ISR_IDLE)
#define USART_FLAG_FE 			(1 << USART_ISR_FE)
#define USART_FLAG_NE 			(1 << USART_ISR_NE)
#define USART_FLAG_PE 			(1 << USART_ISR_PE)

/*
 * USART Interrupt Configuration Macros
 */
#define USART_IT_RXNE    5   // RXNEIE is bit 5 in the CR1 register
#define USART_IT_TXE     7   // TXEIE is bit 7 in the CR1 register

/*********************************************************************
 * 						APIs supported by this driver
 *********************************************************************/
/*
 * Peripheral Clock setup
 */
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void USART_Init(USART_Handle_t *pUSARTHandle);
void USART_DeInit(USART_RegDef_t *pUSARTx);

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate);
void USART_InterruptControl(USART_RegDef_t *pUSARTx, uint8_t interrupt_type, uint8_t EnOrDi);

/*
 * Data Send and Receive
 */
void USART_SendData(USART_RegDef_t *pUSARTx,uint8_t *pTxBuffer, uint32_t Len);
void USART_ReceiveData(USART_RegDef_t *pUSARTx, uint8_t *pRxBuffer, uint32_t Len);
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Len);
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len);

/*
 * IRQ Configuration and ISR handling
 */
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void USART_IRQHandling(USART_Handle_t *pHandle);

/*
 * Other Peripheral Control APIs
 */
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi);
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx , uint32_t FlagName);
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName);

/*
 * Application callback
 */
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t AppEv);


#endif /* STM32L47XXX_USART_DRIVER_H_ */
