#ifndef __PLATFORM_SYSTEM_DEF_HEADER__
#define __PLATFORM_SYSTEM_DEF_HEADER__

#define SERIAL_BAUD_RATE 9600

// Digital pin modes for the platform abstraction.
#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

// Maximum supported analog and digital pins for the target board.
#define MAX_ANALOG_PIN_COUNT 16
#define MAX_DIGITAL_PIN_COUNT 54

// Enable internal use of pull-up resistors when available.
// If enabled, the SET operation always activate the internal pull-ups of the MCU digital inputs.
#define SET_PULL_UP

// SDA and SCL pin mapping for soft I2C implementation.
#define I2C_SDA_PIN   20
#define I2C_SCL_PIN   21

// MOSI, MISO, SCK and CS pin mapping for soft SPI implementation.
#define SPI_MOSI_PIN  22
#define SPI_MISO_PIN  23
#define SPI_SCK_PIN   24
#define SPI_CS_PIN    25

#endif /* __PLATFORM_SYSTEM_DEF_HEADER__ */