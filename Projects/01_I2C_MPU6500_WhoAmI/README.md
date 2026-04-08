# 01 - I2C MPU6500 WHO_AM_I Scanner

## Overview
This bare-metal application verifies I2C communication with an MPU6500 6-axis IMU. It uses a custom interrupt-driven I2C driver to read the `WHO_AM_I` register (`0x75`). Once the data (`0x70`) is captured, it is handed off to a custom USART driver and transmitted to a PC Serial Monitor.

## Hardware Wiring
| STM32L476RG Pin | Peripheral | MPU6500 / PC   | Notes                                    |
| **3V3**         | Power      | VCC            | Do not use 5V!                           |
| **GND**         | Ground     | GND            | Must share ground with Arduino/PC bridge |
| **PB6**         | I2C1 SCL   | SCL            | AF4, Open Drain                          |
| **PB7**         | I2C1 SDA   | SDA            | AF4, Open Drain                          |
| **PA9**         | USART1 TX  | Arduino Pin 10 | AF7, Push-Pull, 9600 Baud                |

Note: The Arduino is acting purely as a UART-to-USB bridge to view the data on a PC monitor.
