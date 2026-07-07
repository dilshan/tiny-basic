# Arduino Due

The *Arduino Due* is the flagship microcontroller board for this project, based on the powerful 32-bit ARM Cortex-M3 processor (SAM3X8E). Thanks to its abundant memory and high processing speed, it serves as the ultimate playground for running Tiny Basic for Arduino.

!!! warning "CRITICAL VOLTAGE WARNING"
    * Unlike older 5V Arduino boards (like the UNO or Mega), the Arduino Due operates strictly at **3.3V**.
    * All I/O pins are **not 5V tolerant**.
    * Connecting 5V signals to any pin can permanently damage the board. **Always use logic level shifters** when interfacing with 5V components.

## Platform Support Status

* **Status:** Default Development Board
* **Compatibility:** 100%. All core features and extended functionalities of Tiny Basic for Arduino are fully implemented and optimized for this board.
* **Program Capacity:** Supports up to 1024 lines of Tiny Basic code in memory.

## Pin Configuration

When connecting peripherals, sensors, or displays to your Tiny Basic setup, use the following designated hardware pins:

### I2C Interface

* **SDA (Data):** Pin 20
* **SCL (Clock):** Pin 21

### SPI Interface

* **MOSI (Master Out Slave In):** Pin 22
* **MISO (Master In Slave Out):** Pin 23
* **SCK (Serial Clock):** Pin 24
* **CS (Chip Select):** Pin 25

### Audio/Tone Output

* **Tone Pin:** Pin 12 *(Use this pin for buzzer)*

## Board Capabilities & I/O Limits

| Resource | Maximum Count | Notes |
| --- | --- | --- |
| Digital I/O Pins  | 54 | Pins 0 to 53 available (3.3V logic only) |
| Analog Input Pins | 16 | 12-bit resolution available |

## Connecting to your Host Computer

To upload your Tiny Basic firmware and access the interactive command line / editor, connect your computer to the board using the *Programming Port* (the micro-USB port closest to the DC power jack, managed by the ATMEGA16U2 chip).

Once connected, open your preferred serial terminal emulator (e.g., *Arduino Serial Monitor*, *PuTTY*, or *minicom*) and select the corresponding COM port to start coding in Tiny Basic for Arduino.

## See also

* [Arduino Due Product Page](https://docs.arduino.cc/hardware/due/)
* [Microchip ATSAM3X8E Product Page](https://www.microchip.com/en-us/product/atsam3x8e)