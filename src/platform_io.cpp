// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef DEBUG

#include <Arduino.h>

#include "platform_io.h"
#include "platform_def.h"
#include "platform.h"

int getAnalogPortValue(int pin)
{
  // On Arduino Due, the analog pins are labeled A0-A15 and correspond to digital pins 54-69.
  if(pin < 0 || pin > (MAX_ANALOG_PIN_COUNT - 1)) {
    err_print("Invalid analog pin: A%d\n", pin);
    return -1;
  }

  int value =  analogRead(A0 + pin);
  return value;
}

void setPinMode(int pin, int mode)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print("Invalid pin: %d\n", pin);
    return;
  }

  if (mode != PIN_MODE_INPUT && mode != PIN_MODE_OUTPUT) {
    err_print("Invalid mode for pin %d\n", pin);
    return;
  }

#ifdef SET_PULL_UP
  if(mode == PIN_MODE_INPUT) {
    mode = INPUT_PULLUP;
  }
#endif

  pinMode(pin, mode);
}

void setPinValue(int pin, int value)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print("Invalid pin: %d\n", pin);
    return;
  }

  int pinValue = (value > 0) ? HIGH : LOW;
  digitalWrite(pin, pinValue);
}

int getPinValue(int pin)
{
  if (pin < 0 || pin > (MAX_DIGITAL_PIN_COUNT - 1)) {
    err_print("Invalid pin: %d\n", pin);
    return -1;
  }

  return digitalRead(pin);
}

int isKeyPressed()
{
  if(Serial.available()) {
    return Serial.read();
  }

  return -1;
}

#endif