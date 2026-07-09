// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "platform_chardrv.h"
#include "platform_def.h"

/**
 * @brief Formats and writes a message to the serial console.
 *
 * @param format printf-style format string.
 * @param ... Optional values used by the format string.
 * @return Number of characters produced by the formatted output.
 */
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

/**
 * @brief Writes a single character to the serial console.
 *
 * @param c Character to emit.
 */
void PlatformCharDrv::printChar(char c)
{
  Serial.print(c);
}

/**
 * @brief Reads an integer from the serial input stream.
 *
 * The input is echoed back to the console while being collected. Newline
 * characters are ignored, carriage return terminates the entry, and backspace
 * removes the last digit.
 *
 * @return Parsed integer value entered by the user.
 */
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

/**
 * @brief Checks whether a character is pending in the serial input buffer.
 *
 * @return The next available character if input is pending, otherwise -1.
 */
int PlatformCharDrv::isKeyPressed()
{
  if(Serial.available()) {
    return Serial.read();
  }

  return -1;
}

#endif