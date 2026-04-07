#ifndef GPO_NEOPIXEL_H_
#define GPO_NEOPIXEL_H_

#include "stm32l47xxx.h"

#define NEOPIXEL_COUNT	8
#define NEO_RESET_US	60


/*
 * Sends 8 bits to the Neopixel using bit-banging.
 * Logic: WS2812B doesn't care about 0V/3V; it cares about TIME.
 * A '1' bit is HIGH for a long time. A '0' bit is HIGH for a short time
 */
void GPO_Neo_SendByte(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t byte);



/*
 * @brief Packages R, G, and B into the 24-bit sequence Neopixels expect
 */
void GPO_Neo_SendColor(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t r, uint8_t g, uint8_t b);



/*
 * @brief Fills the entire bank of 8 LEDs with one color
 */
void GPO_Neo_FillStrip(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t r, uint8_t g, uint8_t b);


/*
 * Sends (0, 0, 0) to all to all LEDs and generates reset pulse
 */
void GPO_Neo_Clear(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber);


#endif /* GPO_NEOPIXEL_H_ */
