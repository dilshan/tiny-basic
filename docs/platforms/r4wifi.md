# Arduino UNO R4 WiFi

The *Arduino UNO R4 WiFi* merges the classic form factor of the UNO with the modern processing power of a 32-bit microcontroller (the Renesas RA4M1) and wireless connectivity via an ESP32-S3 module. It brings increased speed and memory to the traditional layout, making it an excellent compact platform for running Tiny Basic projects with modern hardware capabilities.

## Platform Support Status

* **Status:** Fully Supported
* **Compatibility:** 100%. All functionalities and commands of Tiny Basic for Arduino are working and supported on this board.
* **Program Capacity:** Supports up to **264 lines** of Tiny Basic code in memory.

## Pin Configuration

When connecting peripherals or sensors to your Tiny Basic environment on the UNO R4 WiFi, use the following designated hardware pins:

### I2C Interface

* **SDA (Data):** Pin 5
* **SCL (Clock):** Pin 6

!!! note 
    The I2C pins on this board **require external pull-up resistors** (typically 4.7kΩ to 5V or 3.3V, depending on your peripheral) for reliable communication, as internal pull-ups are not sufficient for standard I2C operation.

### SPI Interface

* **MOSI (Master Out Slave In):** Pin 8
* **MISO (Master In Slave Out):** Pin 9
* **SCK (Serial Clock):** Pin 10
* **CS (Chip Select):** Pin 11

### Audio/Tone Output

* **Tone Pin:** Pin 12 *(Use this pin for buzzer)*

## Board Capabilities & I/O Limits

| Resource | Maximum Count | Notes |
| --- | --- | --- |
| Digital I/O Pins  | 14 | Digital pins 0 to 13 |
| Analog Input Pins | 6 | Analog pins A0 to A5 |

## Connecting to your Host Computer

To upload code or interact with the Tiny Basic command line shell, simply connect the board to your host computer using a standard **USB-C** cable. The onboard USB interface handles both power and serial communication automatically. Open your terminal emulator of choice, select the appropriate COM port, and you are ready to go.

## See also

* [Arduino UNO R4 WiFi Product Page](https://docs.arduino.cc/hardware/uno-r4-wifi/)
* [Renesas RA4M1 Datasheet](https://docs.arduino.cc/resources/datasheets/ra4m1-datasheet.pdf)