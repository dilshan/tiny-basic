// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "error.h"
#include "platform_io.h"
#include "platform_def.h"
#include "platform.h"

/**
 * @brief Reads the value from the specified analog pin.
 * @param pin The analog pin number to read from (0-15 for Arduino Due).
 * @return The analog value read from the pin (0-1023 for Arduino Due).
 *         Returns -1 if the pin number is invalid.
 */
int platformIO::getAnalogPortValue(int pin)
{
  // On Arduino Due, the analog pins are labeled A0-A15 and correspond to digital pins 54-69.
  if(pin < 0 || pin > (MAX_ANALOG_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_ANALOG_PIN, pin);
    return -1;
  }

  int value =  analogRead(A0 + pin);
  return value;
}

/**
 * @brief Sets the PWM value for the specified pin.
 * @param pin The digital pin number to set the PWM value (0-53 for Arduino Due).
 * @param value The PWM value to set (0-255).
 *              A value of 0 corresponds to 0% duty cycle (off),
 *              and a value of 255 corresponds to 100% duty cycle (fully on).
 * @note The pin must be configured as an output and support PWM functionality.
 *       Not all digital pins support PWM. Refer to the Arduino Due documentation for details.  
 */
void platformIO::setPWM(int pin, unsigned char value)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return;
  }

  analogWrite(pin, value);
}

/**
 * @brief Sets the mode of the specified pin (input or output).
 * @param pin The digital pin number to set the mode (0-53 for Arduino Due).
 * @param mode The mode to set for the pin (PIN_MODE_INPUT or PIN_MODE_OUTPUT).
 * @note The pin must be a valid digital pin. Refer to the Arduino Due documentation for details on pin capabilities.
 */
void platformIO::setPinMode(int pin, int mode)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return;
  }

  if (mode != PIN_MODE_INPUT && mode != PIN_MODE_OUTPUT) {
    err_print(ERR_INVALID_PIN_MODE, pin);
    return;
  }

  // If SET_PULL_UP is defined and the mode is set to input, configure the pin with pull-ups.
#ifdef SET_PULL_UP
  if(mode == PIN_MODE_INPUT) {
    mode = INPUT_PULLUP;
  }
#endif

  pinMode(pin, mode);
}

/**
 * @brief Sets the digital value of the specified pin (high or low).
 * @param pin The digital pin number to set the value (0-53 for Arduino Due).
 * @param value The value to set for the pin (0 for LOW, non-zero for HIGH).
 * @note The pin must be configured as an output. Refer to the Arduino Due documentation for details on pin capabilities.
 */
void platformIO::setPinValue(int pin, int value)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return;
  }

  int pinValue = (value == 0) ? LOW : HIGH;
  digitalWrite(pin, pinValue);
}

/**
 * @brief Reads the digital value of the specified pin (high or low).
 * @param pin The digital pin number to read the value from (0-53 for Arduino Due).
 * @return The digital value read from the pin (0 for LOW, 1 for HIGH).
 *         Returns -1 if the pin number is invalid.
 * @note The pin must be configured as an input. Refer to the Arduino Due documentation for details on pin capabilities.
 */
int platformIO::getPinValue(int pin)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return -1;
  }

  return digitalRead(pin);
}

#endif