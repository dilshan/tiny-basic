#ifndef __PLATFORM_SYSTEM_DEF_HEADER__
#define __PLATFORM_SYSTEM_DEF_HEADER__

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

#endif /* __PLATFORM_SYSTEM_DEF_HEADER__ */