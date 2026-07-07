// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "error.h"
#include "platform_io.h"
#include "platform_def.h"
#include "platform.h"

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

void platformIO::setPWM(int pin, unsigned char value)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return;
  }

  analogWrite(pin, value);
}

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

#ifdef SET_PULL_UP
  if(mode == PIN_MODE_INPUT) {
    mode = INPUT_PULLUP;
  }
#endif

  pinMode(pin, mode);
}

void platformIO::setPinValue(int pin, int value)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return;
  }

  int pinValue = (value == 0) ? LOW : HIGH;
  digitalWrite(pin, pinValue);
}

int platformIO::getPinValue(int pin)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print(ERR_INVALID_PIN, pin);
    return -1;
  }

  return digitalRead(pin);
}

#endif