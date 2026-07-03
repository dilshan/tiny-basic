#ifndef DEBUG

#include <Arduino.h>

#include "platform_def.h"
#include "platform_tone.h"

void playTone(int freq, int duration)
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