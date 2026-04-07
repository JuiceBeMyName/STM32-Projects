#ifndef INC_STM32L47XXX_ADC_DRIVER_H_
#define INC_STM32L47XXX_ADC_DRIVER_H_

// This file contains ADC specific data

#include "stm32l47xxx.h"



/*
 * Configuration structure for ADC
 * Focus on potentiometers
 */

typedef struct
{
	uint32_t ADC_Resolution;	/* Possible values from @ADC_RESOLUTIONS */
	uint32_t ADC_SampleTime;	/* Possible values from @ADC_SAMPLE_TIMES */
	uint32_t ADC_Channel;		/* Possible values from @ADC_CHANNELS */
}ADC_Config_t;



/*
 * Handle structure for ADC
 */

typedef struct
{
	ADC_RegDef_t *pADCx;		/* This holds the base address of ADC1-3 */
	ADC_Config_t ADC_Config;	/* This holds the user configuration */
} ADC_Handle_t;



/*
 * @ADC_RESOLUTIONS
 * Found in ADC_CFGR register (RES bits)
 */

#define ADC_RES_12_BIT		0
#define ADC_RES_10_BIT    	1
#define ADC_RES_8_BIT     	2
#define ADC_RES_6_BIT    	3



/*
 * @ADC_SAMPLE_TIMES
 * Found in ADC_SMPR1 and SMPR2
 */

#define ADC_SAMPLE_2_5_CYCLES    0
#define ADC_SAMPLE_6_5_CYCLES    1
#define ADC_SAMPLE_12_5_CYCLES   2
#define ADC_SAMPLE_24_5_CYCLES   3
#define ADC_SAMPLE_47_5_CYCLES   4
#define ADC_SAMPLE_92_5_CYCLES   5
#define ADC_SAMPLE_247_5_CYCLES  6
#define ADC_SAMPLE_640_5_CYCLES  7



/*
 * @ADC_CHANNELS
 * STM32L476 has up to 18 channels
 */

#define ADC_CH_0    	0
#define ADC_CH_1    	1
#define ADC_CH_2    	2
#define ADC_CH_3    	3
#define ADC_CH_4    	4
#define ADC_CH_5    	5
#define ADC_CH_6    	6
#define ADC_CH_7    	7
#define ADC_CH_8    	8
#define ADC_CH_9    	9
#define ADC_CH_10    	10
#define ADC_CH_11   	11
#define ADC_CH_12   	12
#define ADC_CH_13    	13
#define ADC_CH_14    	14
#define ADC_CH_15    	15
#define ADC_CH_16    	16
#define ADC_CH_17    	17
#define ADC_CH_18    	18



/**********************************************************
 * 				APIs supported by this driver
 **********************************************************/

/*
 * Peripheral clock setup
 */

void ADC_PCLK_Control(ADC_RegDef_t *pADCx, uint8_t EnOrDi);



/*
 * Init and DE-init
 */

void ADC_Init(ADC_Handle_t *pADCHandle);
void ADC_DeInit(ADC_RegDef_t *pADCx);



/*
 * Data Read
 */

void ADC_StartConversion(ADC_RegDef_t *pADCx);
uint32_t ADC_GetResult(ADC_RegDef_t *pADCx);


#endif /* INC_STM32L47XXX_ADC_DRIVER_H_ */
