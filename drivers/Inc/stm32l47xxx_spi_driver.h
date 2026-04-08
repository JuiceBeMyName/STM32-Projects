#ifndef STM32L47XXX_SPI_DRIVER_H
#define STM32L47XXX_SPI_DRIVER_H

#include "stm32l47xxx.h"

/*
 * Configuration structure for SPIx peripheral
 */
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
}SPI_Config_t;



/*
 * Handle structure for SPIx peripheral
 */
typedef struct
{
	SPI_RegDef_t	*pSPIx;		/* Base address of SPIx peripheral */
	SPI_Config_t	SPIConfig;
	uint8_t			*pTxBuffer;	/* Store the app Tx buffer address */
	uint8_t			*pRxBuffer;	/* Store the app Rx buffer address */
	__vo uint32_t	TxLen;		/* To store Tx Len */
	__vo uint32_t 	RxLen;		/* To store Rx Len */
	__vo uint8_t	TxState;	/* To store Tx state */
	__vo uint8_t	RxState;	/* To store Rx state */
}SPI_Handle_t ;



/*
 * @SPI_DeviceMode
 * Found in SPI_CR1 register, Bit 2 (MSTR)
 */
#define SPI_DEVICE_MODE_MASTER          1
#define SPI_DEVICE_MODE_SLAVE           0

/*
 * @SPI_BusConfig
 * Custom software states used to configure BIDIMODE (Bit 15) and RXONLY (Bit 10)
 */
#define SPI_BUS_CONFIG_FD               1   // Full Duplex (2 wires: MOSI, MISO)
#define SPI_BUS_CONFIG_HD               2   // Half Duplex (1 wire: Bidirectional)
#define SPI_BUS_CONFIG_SIMPLEX_TXONLY   3   // Simplex TX (1 wire: Transmit only)
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY   4   // Simplex RX (1 wire: Receive only)

/*
 * @SPI_SclkSpeed
 * Found in SPI_CR1 register, Bits 5:3 (BR[2:0])
 * These divide the peripheral clock (PCLK) to set the SPI baud rate.
 */
#define SPI_SCLK_SPEED_DIV2             0   // 000: fPCLK/2
#define SPI_SCLK_SPEED_DIV4             1   // 001: fPCLK/4
#define SPI_SCLK_SPEED_DIV8             2   // 010: fPCLK/8
#define SPI_SCLK_SPEED_DIV16            3   // 011: fPCLK/16
#define SPI_SCLK_SPEED_DIV32            4   // 100: fPCLK/32
#define SPI_SCLK_SPEED_DIV64            5   // 101: fPCLK/64
#define SPI_SCLK_SPEED_DIV128           6   // 110: fPCLK/128
#define SPI_SCLK_SPEED_DIV256           7   // 111: fPCLK/256

/*
 * @SPI_DFF
 * Data Frame Format (Payload size)
 * Mapped to CR2 register, Bits 11:8 (DS[3:0]) on STM32L4
 */
#define SPI_DFF_8BITS                   0
#define SPI_DFF_16BITS                  1

/*
 * @SPI_CPOL
 * Found in SPI_CR1 register, Bit 1 (CPOL)
 */
#define SPI_CPOL_LOW                    0   // Clock idles at 0V
#define SPI_CPOL_HIGH                   1   // Clock idles at 3.3V

/*
 * @SPI_CPHA
 * Found in SPI_CR1 register, Bit 0 (CPHA)
 */
#define SPI_CPHA_LOW                    0   // Data captured on the 1st clock edge
#define SPI_CPHA_HIGH                   1   // Data captured on the 2nd clock edge

/*
 * @SPI_SSM
 * Found in SPI_CR1 register, Bit 9 (SSM)
 */
#define SPI_SSM_HW                      0   // Hardware manages the NSS pin physically
#define SPI_SSM_SW                      1   // Software manages the NSS internally (using SSI bit)



/*
 * SPI related status flags definitions
 */
#define SPI_TXE_FLAG 	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG	(1 << SPI_SR_RXNE)
#define SPI_BSY_FLAG	(1 << SPI_SR_BSY)


/*
 * SPI app States
 */
#define SPI_READY			0
#define SPI_BUSY_IN_RX		1
#define SPI_BUSY_IN_TX		2



/*
 * Possible SPI app events
 */
#define SPI_EVENT_TX_CMPLT		1
#define SPI_EVENT_RX_CMPLT		2
#define SPI_EVENT_OVR_ERR		3
#define SPI_EVENT_CRC_ERR		4



/*********************************************************************
 * 						APIs supported by this driver
 *********************************************************************/
/*
 * Peripheral Clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);



/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);



/*
 * Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);



/*
 * IQR Configuration and ISR handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);



/*
 * Other Peripheral Control APIs
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);



/*
 * Application callback
 */
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);

#endif /* STM32L47XXX_SPI_DRIVER_H */
