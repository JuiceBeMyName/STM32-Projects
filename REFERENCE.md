# Engineering Reference: GPIO Pin Configurations

Understanding exactly how to configure the `OPType` (Output Type) and `PuPd` (Pull-Up/Pull-Down) registers is critical for establishing stable hardware communication. 

### Two Golden Rules:
1. **Inputs don't push:** If a pin is configured as an Input (Button, ADC), the `OPType` register is ignored by the hardware.
2. **I2C Pull-Ups are weak:** The internal STM32 `Pull-Up` is ~40k ohms. This is often too weak for fast I2C. Professional PCBs always use physical 4.7k ohm external resistors for I2C lines.

---

### The Master Configuration Table

| Peripheral / Pin Type | `OPType` (Output) | `PuPd` (Internal Resistor) | Why do we use this exact configuration? |
| :--- | :--- | :--- | :--- |
| **Basic LED / Relay** | `Push-Pull` | `None` | You actively blast 3.3V (Push) to turn it on, and actively sink it to 0V (Pull) to turn it off. Internal springs (PuPd) are useless when actively driven. |
| **Basic Button** | `N/A` *(Input)* | `Pull-Up` or `Pull-Down` | If unpressed, the wire acts like an antenna and absorbs static. A Pull-Up/Down acts like a bungee cord to hold the line at a safe default state. |
| **Analog Input (ADC)**| `N/A` *(Input)* | **`None` (CRITICAL)** | You must measure the *exact* raw physical voltage. Turning on an internal Pull-Up injects extra voltage into the wire and ruins the math. |
| **USART (TX & RX)** | `Push-Pull` | `Pull-Up` | TX uses Push-Pull for sharp square waves. We add a Pull-Up because USART "Idles" HIGH. If a wire unplugs, the Pull-Up prevents the chip from reading floating garbage. |
| **SPI (MOSI, MISO, SCK)**| `Push-Pull` | `None` | SPI is built for maximum speed (MHz). Push-Pull aggressively forces the voltage High and Low to create lightning-fast, perfect square waves. |
| **SPI (Chip Select / CS)**| `Push-Pull` | `None` | The STM32 actively drives this HIGH (sleep) or LOW (wake). *Note: Some engineers add a Pull-Up just to ensure sensors stay asleep while the STM32 boots up.* |
| **I2C (SDA & SCL)** | `Open-Drain` | `Pull-Up` | **CRITICAL:** I2C devices share wires. If two chips used Push-Pull and fought (one pulling 3.3V, one pulling 0V), it would short circuit! Open-Drain ensures chips only pull LOW. |
