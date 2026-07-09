// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_def.h"
#include "platform_tone.h"

/**
 * @brief Generates a square wave of the specified frequency and duration on the TONE_PIN.
 * @param freq Frequency of the tone in Hertz (Hz). Must be greater than or equal to 31 Hz. If less than 31 Hz, the function will simply delay for the specified duration without generating a tone.
 * @param duration Duration of the tone in milliseconds (ms). If less than or equal to 0, the function will return immediately without generating a tone.
 * @note The TONE_PIN must be configured as an output pin before calling this function.
 */
void platformTone::play(int freq, int duration)
{
  pinMode(TONE_PIN, OUTPUT);
  
  if (freq < 31)
  {
    delay(duration);
    return;
  }

  if(duration <= 0)
  {
    return;
  }

  long period = 1000000 / freq;
  long halfPeriod = period / 2;
  unsigned long startTime = millis();

  while ((millis() - startTime) < (unsigned long)duration)
  {
    digitalWrite(TONE_PIN, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(TONE_PIN, LOW);
    delayMicroseconds(halfPeriod);
  }
}

#endif