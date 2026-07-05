// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "platform_chardrv.h"
#include "platform_def.h"

// Print formatted text to the Serial interface.
int PlatformCharDrv::print(const char *format, ...)
{
  char buffer[MAX_LINE_LEN];
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  Serial.print(buffer);

  return len;
}

void PlatformCharDrv::printChar(char c)
{
  Serial.print(c);
}

// Read an integer from Serial input, echoing characters back to the user.
int PlatformCharDrv::input()
{
  char numBuffer[16];
  size_t idx = 0;

  while (true)
  {
    if (Serial.available())
    {
      char c = Serial.read();

      if (c == '\n')
        continue; // Ignore newline characters; only CR terminates input.

      if (c == '\r')
      {
        numBuffer[idx] = '\0';
        Serial.println();
        return atoi(numBuffer);
      }

      if ((c == '\b' || c == 127) && idx > 0)
      {
        // Handle a backspace/delete by removing last digit.
        idx--;
        Serial.print("\b \b");
        continue;
      }

      // Accept only digits and an optional leading minus sign.
      if ((idx < sizeof(numBuffer) - 1) && (isdigit(static_cast<unsigned char>(c)) || (c == '-' && idx == 0)))
      {
        numBuffer[idx++] = c;
        Serial.print(c);
      }
    }
    else
    {
      // Yield to the board's background tasks (Wi-Fi stack, watchdog,
      // etc.) while waiting for input on boards with a cooperative
      // scheduler. This is a no-op on boards that don't need it.
      yield();
    }
  }
}

int PlatformCharDrv::isKeyPressed()
{
  if(Serial.available()) {
    return Serial.read();
  }

  return -1;
}

#endif