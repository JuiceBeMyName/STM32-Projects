# 02 - Bare-Metal ADC Potentiometer & USART Bridge

## Overview
This bare-metal application for the **STM32L476RG** demonstrates an entirely non-blocking, interrupt-driven architecture. It uses the ADC to read analog voltage from a potentiometer and dynamically updates three state LEDs (Green, Yellow, Red) based on configured thresholds. The raw ADC data (10-bit resolution) is continuously transmitted over USART to a PC via an Arduino acting as a UART-to-USB bridge. A 1ms SysTick master clock handles all system timing and alarm blinking.

## Hardware Wiring

### Components to STM32L476RG
| Component | STM32L476RG Pin | Nucleo Header Location | Notes |
| :--- | :--- | :--- | :--- |
| **Potentiometer (Wiper)** | `PA0` (ADC1_IN5) | `A0` | Analog Input |
| **Green LED** | `PC0` | `A5` | Turns on < 340 |
| **Yellow LED** | `PC1` | `A4` | Turns on 340 - 679 |
| **Red LED** | `PC2` | Morpho `CN7`, Pin 35 | Solid > 680, Blinks > 900 |
| **GND** | `GND` | Any GND | Common ground for all components |

### STM32 to Arduino Bridge (UART-to-USB)
*Note: The Arduino requires a simple `SoftwareSerial` pass-through sketch loaded onto it.*

| STM32L476RG | Arduino Uno | Notes |
| :--- | :--- | :--- |
| `PA9` (USART1 TX) | Pin 10 (Software RX) | AF7, Push-Pull, 9600 Baud |
| `PA10` (USART1 RX) | Pin 11 (Software TX) | AF7, Push-Pull, 9600 Baud |
| `GND` | `GND` | **Critical:** Must share common ground |

## Software Architecture
This project relies entirely on a custom Hardware Abstraction Layer (HAL) written from scratch, interacting directly with the ARM Cortex-M4 memory-mapped registers. 

* **`ADC1_2_IRQHandler`**: Triggers upon End of Conversion (EOC). Reads the data register instantly and updates the LED states.
* **`SysTick_Handler`**: Fires every 1ms. Drives the main system clock and handles the 500ms non-blocking toggle for the Red LED alarm state.
* **Non-Blocking Super Loop**: The `while(1)` loop relies on the SysTick timestamp to transmit USART data every 250ms and trigger the next background ADC conversion. 

---

### ⚠️ STM32L4 Hardware Quirks & Lessons Learned
Writing bare-metal drivers for the ultra-low-power STM32L4 architecture requires navigating several silicon-specific traps. The following quirks were patched into the custom drivers during this project:

* **The Analog Switch (`ASCR`):** Setting a pin to analog mode isn't enough. You must manually close the physical switch via the Analog Switch Control Register (`ASCR`) to connect the pin to the ADC.
* **Deep Power Down:** The ADC boots completely disconnected from power. It requires a manual 5-step wake-up sequence and a "dumb" CPU loop delay (to avoid SysTick conflicts) to stabilize the internal voltage regulator.
* **ADC Clock Routing:** Standard APB/AHB clock enables are insufficient. The System Clock must be explicitly routed to the ADC multiplexer via the `RCC_CCIPR` register.
* **Race Conditions:** Hardware timer interrupts (SysTick) and peripheral interrupts (ADC) will silently overwrite each other's GPIO commands if shared states aren't perfectly isolated.
