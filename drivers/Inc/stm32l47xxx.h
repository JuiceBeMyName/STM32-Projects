#ifndef INC_STM32L47XXX_H_
#define INC_STM32L47XXX_H_

// This file contains MCU specific data

#include <stdint.h>
#include <stddef.h>

#define __vo volatile
#define __weak __attribute__((weak))

/************************ Processor Specific Details *********************************
 *
 * ARM Cortex Mx Processor NVIC ISERx register Addresses
 * 0xE000E100 is just a number
 * But registers exist at memory addresses
 * To access that address, we must treat it as a pointer to memory
 */

#define NVIC_ISER0			(__vo uint32_t*) 0xE000E100
#define NVIC_ISER1			(__vo uint32_t*) 0xE000E104
#define NVIC_ISER2			(__vo uint32_t*) 0xE000E108
#define NVIC_ISER3			(__vo uint32_t*) 0xE000E10C

/*
 * ARM Cortex Mx Processor NVIC ICERx register Addresses
 */

#define NVIC_ICER0			(__vo uint32_t*) 0xE000E180
#define NVIC_ICER1			(__vo uint32_t*) 0xE000E184
#define NVIC_ICER2			(__vo uint32_t*) 0xE000E188
#define NVIC_ICER3			(__vo uint32_t*) 0xE000E18C

/*
 * ARM Cortex Mx Processor Priority Register Address Calculation
 */

#define NVIC_PR_BASE_ADDR	(__vo uint32_t*) 0xE000E400


#define NO_PR_BITS_IMPLEMENTED				4



/* SysTick */
#define SYSTICK_BASEADDR	0xE000E010UL

/*
 * SysTick Register Map (Cortex-M4 Core)
 */

typedef struct {
    volatile uint32_t CSR;   	/* 0x00: Control and Status Register */
    volatile uint32_t RVR;   	/* 0x04: Reload Value Register */
    volatile uint32_t CVR;   	/* 0x08: Current Value Register */
    volatile uint32_t CALIB; 	/* 0x0C: Calibration Value Register */
} SysTick_TypeDef;

#define SysTick ((SysTick_TypeDef *) SYSTICK_BASEADDR)

/*
 * SysTick CSR (Control and Status) Bit Definitions
 */
#define SysTick_CSR_ENABLE_Pos      0U
#define SysTick_CSR_ENABLE_Msk      (1UL << SysTick_CSR_ENABLE_Pos)

#define SysTick_CSR_TICKINT_Pos     1U
#define SysTick_CSR_TICKINT_Msk     (1UL << SysTick_CSR_TICKINT_Pos)

#define SysTick_CSR_CLKSOURCE_Pos   2U
#define SysTick_CSR_CLKSOURCE_Msk   (1UL << SysTick_CSR_CLKSOURCE_Pos)

#define SysTick_CSR_COUNTFLAG_Pos   16U
#define SysTick_CSR_COUNTFLAG_Msk   (1UL << SysTick_CSR_COUNTFLAG_Pos)

#define SYSTEM_CLOCK_MHZ            4UL  // Default MSI clock



/*
 * Base addresses of Flash and SRAM memories
 */

#define FLASH_BASEADDR					0x08000000U				/* Base address of embedded Flash memory */
#define SRAM1_BASEADDR					0x20000000U				/* Base address of SRAM1 (main system RAM) */
#define SRAM2_BASEADDR					0x20030000U				/* Base address of SRAM2 (additional low-power RAM) */
#define	ROM_BASEADDR					0x1FFF0000U				/* Base address of system memory (bootloader ROM) */
#define	SRAM							SRAM1_BASEADDR			/* Alias for the primary SRAM used by the application */



/*
 * AHBx and APBx Bus Peripheral base addresses
 */

#define	PERIPH_BASEADDR					0x40000000U				/* Base address of peripheral register space */
#define	APB1PERIPH_BASEADDR				PERIPH_BASEADDR			/* Base address of APB1 peripheral bus */
#define APB2PERIPH_BASEADDR				0x40010000U				/* Base address of APB2 peripheral bus */
#define AHB1PERIPH_BASEADDR				0x40020000U				/* Base address of AHB1 peripheral bus */
#define AHB2PERIPH_BASEADDR				0x48000000U				/* Base address of AHB2 peripheral bus */



/*
 * Base addresses of peripherals hanging on AHB1 bus
 */

#define RCC_BASEADDR					(AHB1PERIPH_BASEADDR + 0x1000)		/* Base address of RCC (AHB1 bus) */



/*
 * Base addresses of peripherals hanging on AHB2 bus
 */

#define GPIOA_BASEADDR					(AHB2PERIPH_BASEADDR + 0x0000)		/* Base address of GPIOA peripheral (AHB2 bus) */
#define GPIOB_BASEADDR					(AHB2PERIPH_BASEADDR + 0x0400)		/* Base address of GPIOB peripheral (AHB2 bus) */
#define GPIOC_BASEADDR					(AHB2PERIPH_BASEADDR + 0x0800)		/* Base address of GPIOC peripheral (AHB2 bus) */
#define GPIOD_BASEADDR					(AHB2PERIPH_BASEADDR + 0x0C00)		/* Base address of GPIOD peripheral (AHB2 bus) */
#define GPIOE_BASEADDR					(AHB2PERIPH_BASEADDR + 0x1000)		/* Base address of GPIOE peripheral (AHB2 bus) */
#define GPIOF_BASEADDR					(AHB2PERIPH_BASEADDR + 0x1400)		/* Base address of GPIOF peripheral (AHB2 bus) */
#define GPIOG_BASEADDR					(AHB2PERIPH_BASEADDR + 0x1800)		/* Base address of GPIOG peripheral (AHB2 bus) */
#define GPIOH_BASEADDR					(AHB2PERIPH_BASEADDR + 0x1C00)		/* Base address of GPIOH peripheral (AHB2 bus) */


#define ADC_BASEADDR					0x50040000U

#define ADC1_BASEADDR					(ADC_BASEADDR + 0x000)		/* Base address of ADC1 peripheral */
#define ADC2_BASEADDR					(ADC_BASEADDR + 0x100)		/* Base address of ADC2 peripheral */
#define ADC3_BASEADDR					(ADC_BASEADDR + 0x200)		/* Base address of ADC3 peripheral */
#define ADC_COMMON_BASEADDR				(ADC_BASEADDR + 0x300)		/* Base address of common ADC peripheral */



/*
 * Base addresses of peripherals hanging on APB1 bus
 */

#define I2C1_BASEADDR					(APB1PERIPH_BASEADDR + 0x5400)		/* Base address of I2C1 peripheral (APB1 bus) */
#define I2C2_BASEADDR					(APB1PERIPH_BASEADDR + 0x5800)		/* Base address of I2C2 peripheral (APB1 bus) */
#define I2C3_BASEADDR					(APB1PERIPH_BASEADDR + 0x5C00)		/* Base address of I2C3 peripheral (APB1 bus) */

#define SPI2_BASEADDR					(APB1PERIPH_BASEADDR + 0x3800)		/* Base address of SPI2 peripheral (APB1 bus) */
#define SPI3_BASEADDR					(APB1PERIPH_BASEADDR + 0x3C00)		/* Base address of SPI3 peripheral (APB1 bus) */

#define USART2_BASEADDR					(APB1PERIPH_BASEADDR + 0x4400)		/* Base address of USART2 peripheral (APB1 bus) */
#define USART3_BASEADDR					(APB1PERIPH_BASEADDR + 0x4800)		/* Base address of USART3 peripheral (APB1 bus) */
#define UART4_BASEADDR					(APB1PERIPH_BASEADDR + 0x4C00)		/* Base address of UART4 peripheral (APB1 bus) */
#define UART5_BASEADDR					(APB1PERIPH_BASEADDR + 0x5000)		/* Base address of UART5 peripheral (APB1 bus) */



/*
 * Base addresses of peripherals hanging on APB2 bus
 */

#define SPI1_BASEADDR					(APB2PERIPH_BASEADDR + 0x3000)		/* Base address of SPI1 peripheral (APB2 bus) */
#define USART1_BASEADDR					(APB2PERIPH_BASEADDR + 0x3800)		/* Base address of USART1 peripheral (APB2 bus) */
#define EXTI_BASEADDR					(APB2PERIPH_BASEADDR + 0x0400)		/* Base address of EXTI peripheral (APB2 bus) */
#define SYSCFG_BASEADDR					APB2PERIPH_BASEADDR					/* Base address of SYSCFG peripheral (APB2 bus) */



/************************** Peripheral register definition structures **************************************/
/*
 * Memory map of the GPIO peripheral
 */

typedef struct {
	__vo uint32_t MODER;			/* GPIO port mode register							Address offset: 0x00 */
	__vo uint32_t OTYPER;			/* GPIO port output type register 					Address offset: 0x04 */
	__vo uint32_t OSPEEDR;			/* GPIO port output speed register 					Address offset: 0x08 */
	__vo uint32_t PUPDR;			/* GPIO port pull-up/pull-down register 			Address offset: 0x0C */
	__vo uint32_t IDR;				/* GPIO port input data register 					Address offset: 0x10 */
	__vo uint32_t ODR;				/* GPIO port output data register 					Address offset: 0x14 */
	__vo uint32_t BSRR;				/* GPIO port bit set/reset register 				Address offset: 0x18 */
	__vo uint32_t LCKR;				/* GPIO port configuration lock register 			Address offset: 0x1C */
	__vo uint32_t AFR[2];			/* AFR[0]: GPIO alternate function low register, AFR[1]: GPIO alternate function high register		Address offset: 0x20, 0x24 */
}GPIO_RegDef_t;

typedef struct {
	__vo uint32_t CR;				/* Clock control register												Address offset: 0x00 */
	__vo uint32_t ICSCR;			/* Internal clock sources calibration register							Address offset: 0x04 */
	__vo uint32_t CFGR;				/* Clock configuration register 										Address offset: 0x08 */
	__vo uint32_t PLLCFGR;			/* PLL configuration register 											Address offset: 0x0C */
	__vo uint32_t PLLSAI1CFGR;		/* PLLSAI1 configuration register										Address offset: 0x10 */
	__vo uint32_t PLLSAI2CFGR;		/* PLLSAI2 configuration register 										Address offset: 0x14 */
	__vo uint32_t CIER;				/* Clock interrupt enable register										Address offset: 0x18 */
	__vo uint32_t CIFR;				/* Clock interrupt flag register										Address offset: 0x1C */
	__vo uint32_t CICR;				/* Clock interrupt clear register 										Address offset: 0x20 */
	uint32_t 	  RESERVED0;		/* Reserved: 0x24 */
	__vo uint32_t AHB1RSTR;			/* AHB1 peripheral reset register										Address offset: 0x28 */
	__vo uint32_t AHB2RSTR;			/* AHB2 peripheral reset register										Address offset: 0x2C */
	__vo uint32_t AHB3RSTR;			/* AHB3 peripheral reset register										Address offset: 0x30 */
	uint32_t 	  RESERVED1;		/* Reserved: 0x34 */
	__vo uint32_t APB1RSTR1;		/* APB1 peripheral reset register 1										Address offset: 0x38 */
	__vo uint32_t APB1RSTR2;		/* APB1 peripheral reset register 2										Address offset: 0x3C */
	__vo uint32_t APB2RSTR;			/* APB2 peripheral reset register										Address offset: 0x40 */
	uint32_t 	  RESERVED2;		/* Reserved: 0x44 */
	__vo uint32_t AHB1ENR;			/* AHB1 peripheral clock enable register								Address offset: 0x48 */
	__vo uint32_t AHB2ENR;			/* AHB2 peripheral clock enable register								Address offset: 0x4C */
	__vo uint32_t AHB3ENR;			/* AHB3 peripheral clock enable register								Address offset: 0x50 */
	uint32_t 	  RESERVED3;		/* Reserved: 0x54 */
	__vo uint32_t APB1ENR1;			/* APB1 peripheral clock enable register 1								Address offset: 0x58 */
	__vo uint32_t APB1ENR2;			/* APB1 peripheral clock enable register 2								Address offset: 0x5C */
	__vo uint32_t APB2ENR;			/* APB2 peripheral clock enable register								Address offset: 0x60 */
	uint32_t 	  RESERVED4;		/* Reserved: 0x64 */
	__vo uint32_t AHB1SMENR;		/* AHB1 peripheral clocks enable in Sleep and Stop modes register		Address offset: 0x68 */
	__vo uint32_t AHB2SMENR;		/* AHB2 peripheral clocks enable in Sleep and Stop modes register		Address offset: 0x6C */
	__vo uint32_t AHB3SMENR;		/* AHB3 peripheral clocks enable in Sleep and Stop modes register		Address offset: 0x70 */
	uint32_t 	  RESERVED5;		/* Reserved: 0x74 */
	__vo uint32_t APB1SMENR1;		/* APB1 peripheral clocks enable in Sleep and Stop modes register 1		Address offset: 0x78 */
	__vo uint32_t APB1SMENR2;		/* APB1 peripheral clocks enable in Sleep and Stop modes register 2		Address offset: 0x7C */
	__vo uint32_t APB2SMENR;		/* APB2 peripheral clocks enable in Sleep and Stop modes register		Address offset: 0x80 */
	uint32_t 	  RESERVED6;		/* Reserved: 0x84 */
	__vo uint32_t CCIPR;			/* Peripherals independent clock configuration register					Address offset: 0x88 */
	uint32_t 	  RESERVED7;		/* Reserved: 0x8C */
	__vo uint32_t BDCR;				/* Backup domain control register										Address offset: 0x90 */
	__vo uint32_t CSR;				/* Control/status register												Address offset: 0x94 */
	__vo uint32_t CRRCR;			/* Clock recovery RC register											Address offset: 0x98 */
	__vo uint32_t CCIPR2;			/* Peripherals independent clock configuration register					Address offset: 0x9C */
}RCC_RegDef_t;



/*
 * Peripheral register definition structure for EXTI
 */

typedef struct
{
	__vo uint32_t IMR1;		/* Interrupt mask register 1				Offset: 0x00 */
	__vo uint32_t EMR1;		/* Event mask register 1					Offset: 0x04 */
	__vo uint32_t RTSR1;	/* Rising trigger selection register 1		Offset: 0x08 */
	__vo uint32_t FTSR1;	/* Falling trigger selection register 1		Offset: 0x0C */
	__vo uint32_t SWIER1;	/* Software interrupt event register 1		Offset: 0x10 */
	__vo uint32_t PR1;		/* Pending register 1						Offset: 0x14 */
	__vo uint32_t IMR2;		/* Interrupt mask register 2				Offset: 0x20 */
	__vo uint32_t EMR2;		/* Event mask register 2					Offset: 0x24 */
	__vo uint32_t RTSR2;	/* Rising trigger selection register 2		Offset: 0x28 */
	__vo uint32_t FTSR2;	/* Falling trigger selection register 2		Offset: 0x2C */
	__vo uint32_t SWIER2;	/* Software interrupt event register 2		Offset: 0x30 */
	__vo uint32_t PR2;		/* Pending register 2						Offset: 0x34 */
}EXTI_RegDef_t;



/*
 * Peripheral register definition for SPI
 */
typedef struct
{
	__vo uint32_t CR1;			/* 0x00: SPI control register 1*/
	__vo uint32_t CR2;			/* 0x04: SPI control register 2 */
	__vo uint32_t SR;			/* 0x08: SPI status register */
	__vo uint32_t DR;			/* 0x0C: SPI data register */
	__vo uint32_t CRCPR;		/* 0x10: SPI CRC polynomial register */
	__vo uint32_t RXCRCR;		/* 0x14: SPI Rx CRC register */
	__vo uint32_t TXCRCR;		/* 0x18: SPI Tx CRC register */
} SPI_RegDef_t;



/*
 * Peripheral register definition structure for SYSCFG
 */
typedef struct
{
	__vo uint32_t MEMRMP;		/* memory remap register								Offset: 0x00 */
	__vo uint32_t CFGR1;		/* configuration register 1								Offset: 0x04 */
	__vo uint32_t EXTICR[4];	/* external interrupt configuration register 1-4		Offset: 0x08-0x14 */
	__vo uint32_t SCSR;			/* SRAM2 control and status register					Offset: 0x18 */
	__vo uint32_t CFGR2;		/* configuration register 2								Offset: 0x1C */
	__vo uint32_t SWPR;			/* SRAM2 write protection register						Offset: 0x20 */
	__vo uint32_t SKR;			/* SYSCFG SRAM2 key register							Offset: 0x24 */
	__vo uint32_t SWPR2;		/* SRAM2 write protection register 2					Offset: 0x28 */
}SYSCFG_RegDef_t;



/*
 * Peripheral register definition for ADC
 * Register map up to DR (Data Register), because it's the highest offset we plan to use
 * ADC common register has 0x300 offset
 */

typedef struct
{
	__vo uint32_t ISR;			/* ADC interrupt and status register             Address offset: 0x00 */
	__vo uint32_t IER;			/* ADC interrupt enable register            	 Address offset: 0x04 */
	__vo uint32_t CR;			/* ADC control register             		     Address offset: 0x08 */
	__vo uint32_t CFGR;			/* ADC configuration register 			         Address offset: 0x0C */
	__vo uint32_t CFGR2;		/* ADC configuration register 2		             Address offset: 0x10 */
	__vo uint32_t SMPR[2];		/* ADC sample time register 1 and 2		         Address offset: 0x14 - 0x18 */
	uint32_t 	  RESERVED0;	/* 0x1C 													  				 */
	__vo uint32_t TR1[3];			/* ADC watchdog threshold register 1-3       Address offset: 0x20 - 0x28 */
	uint32_t 	  RESERVED1;	/* 0x2C 													  				 */
	__vo uint32_t SQR[4];		/* ADC regular sequence register 1-4	         Address offset: 0x30 - 0x3C */
	__vo uint32_t DR;			/* ADC regular data register		             Address offset: 0x40 */
}ADC_RegDef_t;

typedef struct
{
	__vo uint32_t CSR;    		/* Common status register 							Offset: 0x300 */
	uint32_t      RESERVED;		/* 0x04 													      */
	__vo uint32_t CCR;    		/* Common control register 							Offset: 0x308 */
	__vo uint32_t CDR;    		/* Common regular data register for dual mode 		Offset: 0x30C */
} ADC_Common_RegDef_t;



/*
 * I2C peripheral register definition structure
 */
typedef struct
{
	__vo uint32_t CR1;			/* 0x00: I2C control register 1 */
	__vo uint32_t CR2;			/* 0x04: I2C control register 2 */
	__vo uint32_t OAR1;			/* 0x08: I2C own address 1 register */
	__vo uint32_t OAR2;			/* 0x0C: I2C own address 2 register */
	__vo uint32_t TIMINGR;		/* 0x10: I2C timing register */
	__vo uint32_t TIMEOUTR;		/* 0x14: I2C timeout register*/
	__vo uint32_t ISR;			/* 0x18: I2C interrupt and status register */
	__vo uint32_t ICR;			/* 0x1C: I2C interrupt clear register */
	__vo uint32_t PECR;			/* 0x20: I2C PEC register */
	__vo uint32_t RXDR;			/* 0x24: I2C receive data register */
	__vo uint32_t TXDR;			/* 0x28: I2C transmit data register */
} I2C_RegDef_t;



/*
 * USART peripheral register definition structure
 */
typedef struct
{
	__vo uint32_t CR1;		/* 0x00: USART control register 1 */
	__vo uint32_t CR2;		/* 0x04: USART control register 2 */
	__vo uint32_t CR3;		/* 0x08: USART control register 3 */
	__vo uint32_t BRR;		/* 0x0C: USART baud rate register */
	__vo uint32_t GTPR;		/* 0x10: USART guard time and prescaler register */
	__vo uint32_t RTOR;		/* 0x14: USART receiver timeout register */
	__vo uint32_t RQR;		/* 0x18: USART request register */
	__vo uint32_t ISR;		/* 0x1C: USART interrupt and status register */
	__vo uint32_t ICR;		/* 0x20: USART interrupt flag clear register */
	__vo uint32_t RDR;		/* 0x24: USART receive data register */
	__vo uint32_t TDR;		/* 0x28: USART transmit data register */
}USART_RegDef_t;


/*
 * Register Bit Definitions for ADC
 */

// ADC Control Register (ADC_CR) bits
#define ADC_CR_ADEN_POS			(0U)	/* ADEN: ADC enable control */
#define ADC_CR_ADSTART_POS     	(2U)	/* ADSTART: ADC start of regular conversion */
#define ADC_CR_ADVREGEN_POS    	(28U)	/* ADVREGEN: ADC voltage regulator enable */
#define ADC_CR_DEEPPWD_POS     	(29U)	/* DEEPPWD: Deep-power-down enable */
#define ADC_CR_ADCAL_POS       	(31U)	/* ADCAL: ADC calibration */

// ADC Status Register (ADC_ISR) bits
#define ADC_ISR_ADRDY_POS      	(0U)	/* ADRDY: ADC ready */
#define ADC_ISR_EOC_POS        	(2U)	/* EOC: End of conversion flag */

// ADC Common Control Register (ADC_CCR) bits
#define ADC_CCR_CKMODE_POS		(16U)	/* CKMODE[1:0]: ADC clock mode */


/*
 * Peripheral definitions (peripheral base addresses typecasted to xxx_RegDef_t)
 * We typecast GPIOA_BASEADDR to GPIO_RegDef_t* so that:
 * A raw memory address becomes a “structured view” of the GPIO registers
 * Treat this number as a pointer to a GPIO_RegDef_t struct
 * Go to memory address (0x48000000) and access the field MODER at offset 0x00
 * By casting to a struct pointer, the compiler can:
 * Add the proper offsets automatically
 */

#define GPIOA			((GPIO_RegDef_t*) GPIOA_BASEADDR)
#define GPIOB			((GPIO_RegDef_t*) GPIOB_BASEADDR)
#define GPIOC			((GPIO_RegDef_t*) GPIOC_BASEADDR)
#define GPIOD			((GPIO_RegDef_t*) GPIOD_BASEADDR)
#define GPIOE			((GPIO_RegDef_t*) GPIOE_BASEADDR)
#define GPIOF			((GPIO_RegDef_t*) GPIOF_BASEADDR)
#define GPIOG			((GPIO_RegDef_t*) GPIOG_BASEADDR)
#define GPIOH			((GPIO_RegDef_t*) GPIOH_BASEADDR)

#define RCC				((RCC_RegDef_t*) RCC_BASEADDR)
#define EXTI			((EXTI_RegDef_t*) EXTI_BASEADDR)
#define SYSCFG			((SYSCFG_RegDef_t*) SYSCFG_BASEADDR)

#define SPI1			((SPI_RegDef_t*) SPI1_BASEADDR)
#define SPI2			((SPI_RegDef_t*) SPI2_BASEADDR)
#define SPI3			((SPI_RegDef_t*) SPI3_BASEADDR)

#define I2C1			((I2C_RegDef_t*) I2C1_BASEADDR)
#define I2C2			((I2C_RegDef_t*) I2C2_BASEADDR)
#define I2C3			((I2C_RegDef_t*) I2C3_BASEADDR)

#define ADC1			((ADC_RegDef_t*) ADC1_BASEADDR)
#define ADC2			((ADC_RegDef_t*) ADC2_BASEADDR)
#define ADC3			((ADC_RegDef_t*) ADC3_BASEADDR)
#define ADC_COM			((ADC_Common_RegDef_t*) ADC_COMMON_BASEADDR)

#define USART1			((USART_RegDef_t*) USART1_BASEADDR)
#define USART2			((USART_RegDef_t*) USART2_BASEADDR)
#define USART3			((USART_RegDef_t*) USART3_BASEADDR)
#define UART4			((USART_RegDef_t*) UART4_BASEADDR)
#define UART5			((USART_RegDef_t*) UART5_BASEADDR)



/*
 * Clock Enable Macros for GPIOx peripherals
 * PLCK_EN stands for Peripheral Clock Enable
 * We put () at the end because these macros are meant to behave like function calls, not values
 */

#define GPIOA_PCLK_EN()	(RCC->AHB2ENR |= (1 << 0))		/* IO port A clock enable */
#define GPIOB_PCLK_EN()	(RCC->AHB2ENR |= (1 << 1))		/* IO port B clock enable */
#define GPIOC_PCLK_EN()	(RCC->AHB2ENR |= (1 << 2))		/* IO port C clock enable */
#define GPIOD_PCLK_EN()	(RCC->AHB2ENR |= (1 << 3))		/* IO port D clock enable */
#define GPIOE_PCLK_EN()	(RCC->AHB2ENR |= (1 << 4))		/* IO port E clock enable */
#define GPIOF_PCLK_EN()	(RCC->AHB2ENR |= (1 << 5))		/* IO port F clock enable */
#define GPIOG_PCLK_EN()	(RCC->AHB2ENR |= (1 << 6))		/* IO port G clock enable */
#define GPIOH_PCLK_EN()	(RCC->AHB2ENR |= (1 << 7))		/* IO port H clock enable */



/*
 * Clock Enable Macros for I2Cx peripherals
 */

#define I2C1_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 21))	/* I2C1 clock enable */
#define I2C2_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 22))	/* I2C2 clock enable */
#define I2C3_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 23))	/* I2C3 clock enable */



/*
 * Clock Enable Macros for SPIx peripherals
 */

#define SPI1_PCLK_EN()		(RCC->APB2ENR |= (1 << 12))		/* SPI1 clock enable */
#define SPI2_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 14))	/* SPI2 clock enable */
#define SPI3_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 15))	/* SPI3 clock enable */



/*
 * Clock Enable Macros for USARTx peripherals
 */

#define USART1_PCLK_EN()	(RCC->APB2ENR |= (1 << 14))		/* USART1 clock enable*/
#define USART2_PCLK_EN()	(RCC->APB1ENR1 |= (1 << 17))	/* USART2 clock enable */
#define USART3_PCLK_EN()	(RCC->APB1ENR1 |= (1 << 18))	/* USART3 clock enable */
#define UART4_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 19))	/* UART4 clock enable */
#define UART5_PCLK_EN()		(RCC->APB1ENR1 |= (1 << 20))	/* UART5 clock enable */


/*
 * Clock Enable Macros for SYSCFG peripheral
 */

#define SYSCFG_PCLK_EN()	(RCC->APB2ENR |= (1 << 0))		/* SYSCFG clock enable */



/*
 * Clock Enable Macros for ADC Peripheral
 */

#define ADC_PCLK_EN()		(RCC->AHB2ENR |= 1 << 13)		/* ADC clock enable */



/*
 * Clock Disable Macros for GPIOx peripherals
 */

#define GPIOA_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 0))		/* IO port A clock disable */
#define GPIOB_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 1))		/* IO port B clock disable */
#define GPIOC_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 2))		/* IO port C clock disable */
#define GPIOD_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 3))		/* IO port D clock disable */
#define GPIOE_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 4))		/* IO port E clock disable */
#define GPIOF_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 5))		/* IO port F clock disable */
#define GPIOG_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 6))		/* IO port G clock disable */
#define GPIOH_PCLK_DI()	(RCC->AHB2ENR &= ~(1 << 7))		/* IO port H clock disable */



/*
 * Clock Disable Macros for I2Cx peripherals
 */

#define I2C1_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 21))	/* I2C1 clock disable */
#define I2C2_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 22))	/* I2C2 clock disable */
#define I2C3_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 23))	/* I2C3 clock disable */



/*
 * Clock Disable Macros for SPIx peripherals
 */

#define SPI1_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 12))	/* SPI1 clock disable */
#define SPI2_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 14))	/* SPI2 clock disable */
#define SPI3_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 15))	/* SPI3 clock disable */



/*
 * Clock Disable Macros for USARTx peripherals
 */

#define USART1_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 14))	/* USART1 clock disable*/
#define USART2_PCLK_DI()	(RCC->APB1ENR1 &= ~(1 << 17))	/* USART2 clock disable */
#define USART3_PCLK_DI()	(RCC->APB1ENR1 &= ~(1 << 18))	/* USART3 clock disable */
#define UART4_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 19))	/* UART4 clock disable */
#define UART5_PCLK_DI()		(RCC->APB1ENR1 &= ~(1 << 20))	/* UART5 clock disable */



/*
 * Clock Disable Macros for SYSCFG peripherals
 */

#define SYSCFG_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 0))		/* SYSCFG clock disable */



/*
 * Clock Disable Macros for ADC Peripheral
 */
#define ADC_PCLK_DI()		(RCC->AHB2ENR &= ~(1 << 13))	/* ADC clock disable */



/*
 * Macros to reset GPIOx peripherals
 *
 * Assume: RCC->AHB2RSTR = 0x00000000  // all peripherals running
 * Step 1 — assert reset (button clicked and hold)
 * 0000 0000 0000 0000 0000 0000 0000 0001 (binary)
 *
 * Step 2 — release reset (button released)
 * 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * GPIOA comes out of reset
 * Other bits still unchanged
 * while(0) -> macro executed once, reset pulse generated, everything else untouched
 */

#define GPIOA_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 0)); (RCC->AHB2RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 1)); (RCC->AHB2RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 2)); (RCC->AHB2RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 3)); (RCC->AHB2RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 4)); (RCC->AHB2RSTR &= ~(1 << 4)); } while(0)
#define GPIOF_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 5)); (RCC->AHB2RSTR &= ~(1 << 5)); } while(0)
#define GPIOG_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 6)); (RCC->AHB2RSTR &= ~(1 << 6)); } while(0)
#define GPIOH_REG_RESET()	do { (RCC->AHB2RSTR |= (1 << 7)); (RCC->AHB2RSTR &= ~(1 << 7)); } while(0)



/*
 * SPI Peripheral Reset Macros
 */
#define SPI1_REG_RESET()    do { (RCC->APB2RSTR |= (1 << 12)); (RCC->APB2RSTR &= ~(1 << 12)); } while(0)
#define SPI2_REG_RESET()    do { (RCC->APB1RSTR1 |= (1 << 14)); (RCC->APB1RSTR1 &= ~(1 << 14)); } while(0)
#define SPI3_REG_RESET()    do { (RCC->APB1RSTR1 |= (1 << 15)); (RCC->APB1RSTR1 &= ~(1 << 15)); } while(0)



/*
 * Macro to reset ADC
 * 1. Read: The CPU reads current value of register
 * 2. Modify: It applies "Mask" (1111 1111 1111 1111 1110 1111 1111 1111)
 * 3. Write: It writes the result back
 * Result: Only bit 13 changes
 */

#define ADC_REG_RESET()    do { (RCC->AHB2RSTR |= (1 << 13)); (RCC->AHB2RSTR &= ~(1 << 13)); } while(0)



/*
 * Returns port code for given GPIO base address
 */

#define GPIO_BASEADDR_TO_CODE(x)	  ( (x == GPIOA) ? 0 :\
										(x == GPIOB) ? 1 :\
										(x == GPIOC) ? 2 :\
										(x == GPIOD) ? 3 :\
										(x == GPIOE) ? 4 :\
										(x == GPIOF) ? 5 :\
										(x == GPIOG) ? 6 :\
										(x == GPIOH) ? 7 : 0)



/*
 * IRQ (Interrupt request)  Numbers of MCU
 */
#define IRQ_NO_EXTI0		6
#define IRQ_NO_EXTI1		7
#define IRQ_NO_EXTI2		8
#define IRQ_NO_EXTI3		9
#define IRQ_NO_EXTI4		10
#define IRQ_NO_EXTI9_5		23
#define IRQ_NO_EXTI15_10	40

/*
 * IRQ (Interrupt request) Numbers of MCU for SPI
 */
#define IRQ_NO_SPI1			35
#define IRQ_NO_SPI2			36
#define IRQ_NO_SPI3			51

/*
 * IRQ (Interrupt request) Numbers of MCU for I2C
 * EV = Event (Normal communication: TXIS, RXNE, TC)
 * ER = Error (Bus errors: BERR, ARLO, OVR)
 */
#define IRQ_NO_I2C1_EV		31
#define IRQ_NO_I2C1_ER		32
#define IRQ_NO_I2C2_EV		33
#define IRQ_NO_I2C2_ER		34
#define IRQ_NO_I2C3_EV		72
#define IRQ_NO_I2C3_ER		73

/*
 * Macros for all the possible priority levels
 */
#define NVIC_IRQ_PRI0		0
#define NVIC_IRQ_PRI1		1
#define NVIC_IRQ_PRI2		2
#define NVIC_IRQ_PRI3		3
#define NVIC_IRQ_PRI4		4
#define NVIC_IRQ_PRI5		5
#define NVIC_IRQ_PRI6		6
#define NVIC_IRQ_PRI7		7
#define NVIC_IRQ_PRI8		8
#define NVIC_IRQ_PRI9		9
#define NVIC_IRQ_PRI10		10
#define NVIC_IRQ_PRI11		11
#define NVIC_IRQ_PRI12		12
#define NVIC_IRQ_PRI13		13
#define NVIC_IRQ_PRI14		14
#define NVIC_IRQ_PRI15		15

/*
 * IRQ (Interrupt request) Numbers of MCU for USART/UART
 */
#define IRQ_NO_USART1		37
#define IRQ_NO_USART2		38
#define IRQ_NO_USART3		39
#define IRQ_NO_UART4		52
#define IRQ_NO_UART5		53
#define IRQ_NO_LPUART1		70



/*
 * Generic macros for bit manipulation
 */

// Sets a specific bit to 1 (uses OR)
#define SET_BIT(REG, BIT)		((REG) |= (1U << (BIT)))

// Clears a specific bit to 0 (uses AND with a NOT mask)
#define CLEAR_BIT(REG, BIT)		((REG) &= ~(1U << (BIT)))

/*
 * Returns 1 if the bit is set, 0 if it is clear
 * Let's say the Register is 1010 and we want to read Bit 3
 * 1. Shift Right >> 3: The bits move: 0001 (Bit 3 is now at the very end)
 * 2. AND with 1U: 0001 & 0001 = 1
 * 3. Result: You get a 1 if the bit was set, and 0 if it was empty
 */
#define READ_BIT(REG, BIT) 		(((REG) >> (BIT)) & 1U)

// Flips the bit (1 becomes 0, 0 becomes 1)
#define TOGGLE_BIT(REG, BIT)	((REG) ^= (1U << (BIT)))



/*
 * Some generic macros
 */

#define ENABLE				1
#define DISABLE				0
#define SET 				ENABLE
#define RESET 				DISABLE
#define GPIO_PIN_SET		SET
#define GPIO_PIN_RESET		RESET
#define FLAG_SET			SET
#define FLAG_RESET			RESET



/******************************************************************
 * Bit position definitions of SPI peripheral
 ******************************************************************/
/*
 * Bit positions definitions SPI_CR1
 */
#define SPI_CR1_CPHA		0
#define SPI_CR1_CPOL		1
#define SPI_CR1_MSTR		2
#define SPI_CR1_BR			3
#define SPI_CR1_SPE			6
#define SPI_CR1_LSBFIRST	7
#define SPI_CR1_SSI			8
#define SPI_CR1_SSM			9
#define SPI_CR1_RXONLY		10
#define SPI_CR1_CRCL		11
#define SPI_CR1_CRCNEXT		12
#define SPI_CR1_CRCEN		13
#define SPI_CR1_BIDIOE		14
#define SPI_CR1_BIDIMODE	15

/*
 * Bit positions definitions SPI_CR2
 */
#define SPI_CR2_RXDMAEN		0	// Rx buffer DMA enable
#define SPI_CR2_TXDMAEN		1	// Tx buffer DMA enable
#define SPI_CR2_SSOE		2	// SS output enable
#define SPI_CR2_NSSP		3	// NSS pulse management
#define SPI_CR2_FRF			4	// Frame format
#define SPI_CR2_ERRIE		5	// Error interrupt enable
#define SPI_CR2_RXNEIE		6	// RX buffer not empty interrupt enable
#define SPI_CR2_TXEIE		7	// Tx buffer empty interrupt enable
#define SPI_CR2_DS			8	// Data size (Bits 11:8)
#define SPI_CR2_FRXTH		12	// FIFO reception threshold
#define SPI_CR2_LDMA_RX		13	// Last DMA transfer for reception
#define SPI_CR2_LDMA_TX		14	// Last DMA transfer for transmission

/*
 * Bit positions definitions SPI_SR (Status Register)
 */
#define SPI_SR_RXNE			0	// Receive buffer not empty
#define SPI_SR_TXE			1	// Transmit buffer empty
#define SPI_SR_CHSIDE		2	// Channel side (Used in I2S mode)
#define SPI_SR_UDR			3	// Underrun flag
#define SPI_SR_CRCERR		4	// CRC error flag
#define SPI_SR_MODF			5	// Mode fault
#define SPI_SR_OVR			6	// Overrun flag
#define SPI_SR_BSY			7	// Busy flag
#define SPI_SR_FRE			8	// Frame format error
#define SPI_SR_FRLVL		9	// FIFO reception level (Bits 10:9)
#define SPI_SR_FTLVL		11	// FIFO transmission level (Bits 12:11)



/******************************************************************
 * Bit position definitions of I2C peripheral
 ******************************************************************/
/*
 * Bit positions for I2C_CR1
 */
#define I2C_CR1_PE 			0	/* Peripheral enable */
#define I2C_CR1_TXIE 		1	/* TX Interrupt enable */
#define I2C_CR1_RXIE 		2	/* RX Interrupt enable */
#define I2C_CR1_ADDRIE 		3	/* Address match Interrupt enable (slave only) */
#define I2C_CR1_NACKIE 		4	/* Not acknowledge received Interrupt enable */
#define I2C_CR1_STOPIE 		5	/* Stop detection Interrupt enable */
#define I2C_CR1_TCIE		6	/* Transfer Complete interrupt enable */
#define I2C_CR1_ERRIE 		7	/* Error interrupts enable */
#define I2C_CR1_ANFOFF 		12	/* Analog noise filter OFF */
#define I2C_CR1_TXDMAEN 	14	/* DMA transmission requests enable */
#define I2C_CR1_RXDMAEN 	15	/* DMA reception requests enable */

/*
 * Bit positions for I2C_CR2
 */
#define I2C_CR2_SADD          0     // Slave address bit (bits 0:9)
#define I2C_CR2_RD_WRN        10    // Transfer direction (master)
#define I2C_CR2_ADD10         11    // 10-bit addressing mode (master)
#define I2C_CR2_HEAD10R       12    // 10-bit address header only read direction (master)
#define I2C_CR2_START         13    // START generation
#define I2C_CR2_STOP          14    // STOP generation
#define I2C_CR2_NACK          15    // NACK generation (slave)
#define I2C_CR2_NBYTES        16    // Number of bytes (bits 16:23)
#define I2C_CR2_RELOAD        24    // NBYTES reload mode
#define I2C_CR2_AUTOEND       25    // Automatic end mode (master)

/*
 * Bit positions for I2C_ISR (Interrupt and Status Register)
 */
#define I2C_ISR_TXE           0     // Transmit data register empty
#define I2C_ISR_TXIS          1     // Transmit interrupt status
#define I2C_ISR_RXNE          2     // Receive data register not empty
#define I2C_ISR_ADDR          3     // Address matched (slave)
#define I2C_ISR_NACKF         4     // Not acknowledge received flag
#define I2C_ISR_STOPF         5     // Stop detection flag
#define I2C_ISR_TC            6     // Transfer Complete (master)
#define I2C_ISR_TCR           7     // Transfer Complete Reload
#define I2C_ISR_BERR          8     // Bus error
#define I2C_ISR_ARLO          9     // Arbitration lost
#define I2C_ISR_OVR           10    // Overrun/Underrun
#define I2C_ISR_PECERR        11    // PEC Error in reception
#define I2C_ISR_TIMEOUT       12    // Timeout or t_low detection flag
#define I2C_ISR_ALERT         13    // SMBus alert
#define I2C_ISR_BUSY          15    // Bus busy
#define I2C_ISR_DIR           16    // Transfer direction (slave)

/*
 * Bit positions for I2C_OAR1
 */
#define I2C_OAR1_OA1EN		15	// Own Address 1 enable



/******************************************************************
 * Bit position definitions of I2C peripheral
 ******************************************************************/
/*
 * Bit position definitions for USART_CR1
 */
#define USART_CR1_UE        0   // USART Enable
#define USART_CR1_RE        2   // Receiver Enable
#define USART_CR1_TE        3   // Transmitter Enable
#define USART_CR1_IDLEIE    4   // IDLE interrupt enable
#define USART_CR1_RXNEIE    5   // RXNE interrupt enable
#define USART_CR1_TCIE      6   // Transmission complete interrupt enable
#define USART_CR1_TXEIE     7   // TXE interrupt enable
#define USART_CR1_PEIE      8   // PE interrupt enable
#define USART_CR1_PS        9   // Parity selection
#define USART_CR1_PCE       10  // Parity control enable
#define USART_CR1_WAKE      11  // Receiver wakeup method
#define USART_CR1_M0        12  // Word length bit 0
#define USART_CR1_MME       13  // Mute mode enable
#define USART_CR1_CMIE      14  // Character match interrupt enable
#define USART_CR1_OVER8     15  // Oversampling mode
#define USART_CR1_M1        28  // Word length bit 1

/*
 * Bit position definitions for USART_CR2
 */
#define USART_CR2_STOP      12  // STOP bits (takes up 2 bits: 12 and 13)

/*
 * Bit position definitions for USART_CR3
 */
#define USART_CR3_EIE       0   // Error interrupt enable
#define USART_CR3_HDSEL     3   // Half-duplex selection
#define USART_CR3_NACK      4   // Smartcard NACK enable
#define USART_CR3_SCEN      5   // Smartcard mode enable
#define USART_CR3_DMAR      6   // DMA enable receiver
#define USART_CR3_DMAT      7   // DMA enable transmitter
#define USART_CR3_RTSE      8   // RTS enable
#define USART_CR3_CTSE      9   // CTS enable
#define USART_CR3_CTSIE     10  // CTS interrupt enable

/*
 * Bit position definitions for USART_ISR (Interrupt & Status Register)
 */
#define USART_ISR_PE        0   // Parity error
#define USART_ISR_FE        1   // Framing error
#define USART_ISR_NF        2   // Noise detected flag
#define USART_ISR_ORE       3   // Overrun error
#define USART_ISR_IDLE      4   // Idle line detected
#define USART_ISR_RXNE      5   // Read data register not empty
#define USART_ISR_TC        6   // Transmission complete
#define USART_ISR_TXE       7   // Transmit data register empty
#define USART_ISR_CTS       9   // CTS flag
#define USART_ISR_BUSY      16  // Busy flag



#include "stm32l47xxx_gpio_driver.h"
#include "stm32l47xxx_adc_driver.h"
#include "stm32l47xxx_systick.h"
#include "stm32l47xxx_neopixel.h"
#include "stm32l47xxx_spi_driver.h"
#include "stm32l47xxx_i2c_driver.h"
#include "stm32l47xxx_usart_driver.h"
#include "stm32l47xxx_rcc_driver.h"

#endif /* INC_STM32L47XXX_H_ */
