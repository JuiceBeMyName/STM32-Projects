#include "stm32l47xxx_neopixel.h"
#include "stm32l47xxx_systick.h" // For delay_us
#include "stm32l47xxx.h"



/*********************************************************************
 * @fn      		- GPO_Neo_SendByte
 *
 * @brief           - Transmits one byte to a NeoPixel LED using
 *                    software bit-banging with precise timing
 *
 * @param[in]		- pGPIOx: Base address of the GPIO port used as data output
 * @param[in]		- pinNumber: GPIO pin number connected to NeoPixel DIN
 * @param[in]		- byte: 8-bit data to be transmitted (MSB first)
 *
 * @return			- none
 *
 * @note			- Generates WS2812/SK6812 compatible waveform:
 *                    logical '1' = long HIGH pulse
 *                    logical '0' = short HIGH pulse
 *                  Function must not be interrupted during execution,
 *                  otherwise timing corruption will cause LED glitches
 *********************************************************************/
void GPO_Neo_SendByte(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t byte)
{
    // Pre-calculate the masks outside the loop to save CPU time!
    // If pinNumber is 15: setHighMask = bit 15, setLowMask = bit 31
    uint32_t setHighMask = (1 << pinNumber);
    uint32_t setLowMask = (1 << (pinNumber + 16));

    // Loop through every bit in the byte (from most significant to least)
    for (int i = 7; i >= 0; i--) {

        // Check if the current bit of our byte is a 1 or a 0
        if (byte & (1 << i)) {

            /* SEND BIT '1' (Requires a LONG High pulse) */
            pGPIOx->BSRR = setHighMask;     // Pin goes HIGH instantly
            __asm __vo ("nop"); __asm __vo ("nop");     // Wait a bit...
            pGPIOx->BSRR = setLowMask;      // Pin goes LOW instantly
            __asm __vo ("nop");                   // Wait a tiny bit...

        } else {

            /* SEND BIT '0' (Requires a SHORT High pulse) */
            pGPIOx->BSRR = setHighMask;     // Pin goes HIGH instantly
            // NO NOPS! We pull it low immediately because a '0' must be a very short pulse
            pGPIOx->BSRR = setLowMask;      // Pin goes LOW instantly
            __asm __vo ("nop"); __asm __vo ("nop");     // Wait a bit before the next loop
        }
    }
}



/*********************************************************************
 * @fn      		- GPO_Neo_SendColor
 *
 * @brief           - Sends a complete 24-bit RGB color value to a NeoPixel
 *                    by transmitting three sequential bytes in GRB order
 *
 * @param[in]		- pGPIOx: Base address of the GPIO port used as data output
 * @param[in]		- pinNumber: GPIO pin number connected to NeoPixel DIN
 * @param[in]		- r: Red intensity (0–255)
 * @param[in]		- g: Green intensity (0–255)
 * @param[in]		- b: Blue intensity (0–255)
 *
 * @return			- none
 *
 * @note			- NeoPixels require color data in GRB order, not RGB.
 *                    Internally calls GPO_Neo_SendByte() three times.
 *                    Interrupts must remain disabled during transmission
 *                    to preserve timing accuracy
 *********************************************************************/
void GPO_Neo_SendColor(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t r, uint8_t g, uint8_t b)
{
	// Neopixels expect colours in GRB order
	GPO_Neo_SendByte(pGPIOx, pinNumber, g);
	GPO_Neo_SendByte(pGPIOx, pinNumber, r);
	GPO_Neo_SendByte(pGPIOx, pinNumber, b);
}



/*********************************************************************
 * @fn      		- GPO_Neo_FillStrip
 *
 * @brief           - Sets all NeoPixels in the strip to the same RGB color
 *
 * @param[in]		- pGPIOx: Base address of the GPIO port used as data output
 * @param[in]		- pinNumber: GPIO pin number connected to NeoPixel DIN
 * @param[in]		- r: Red intensity (0–255)
 * @param[in]		- g: Green intensity (0–255)
 * @param[in]		- b: Blue intensity (0–255)
 *
 * @return			- none
 *
 * @note			- Sends color data to every LED and then generates the
 *                    required reset pulse (>50us) to latch the values.
 *                    LEDs update simultaneously after the reset delay
 *********************************************************************/
void GPO_Neo_FillStrip(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t r, uint8_t g, uint8_t b)
{
    // 1. DISABLE GLOBAL INTERRUPTS
    // Protects the strict WS2812 timing from the 1ms SysTick heartbeat
    __asm volatile ("cpsid i");

    // 2. Loop through each LED and send the data
    for (int i = 0; i < NEOPIXEL_COUNT; i++) {
        GPO_Neo_SendColor(pGPIOx, pinNumber, r, g, b);
    }

    // 3. RE-ENABLE GLOBAL INTERRUPTS
    // Let the SysTick FSM heartbeat resume immediately
    __asm volatile ("cpsie i");

    // After sending NEOPIXEL_COUNT * 24 bits, we must wait >50us to "latch" the colors in
    // This delay is safe to be interrupted, so it stays outside the protected block
    delay_us(NEO_RESET_US);
}


/*********************************************************************
 * @fn      		- GPO_Neo_Clear
 *
 * @brief           - Turns off all NeoPixels in the strip (sets to black)
 *
 * @param[in]		- pGPIOx: Base address of the GPIO port
 * @param[in]		- pinNumber: GPIO pin number
 *
 * @return			- none
 *
 * @note			- Sends (0,0,0) to all LEDs and generates reset pulse
 *********************************************************************/
void GPO_Neo_Clear(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber)
{
	// Setting intensities to 0 effectively turns the LEDs off
	GPO_Neo_FillStrip(pGPIOx, pinNumber, 0, 0, 0);
}

