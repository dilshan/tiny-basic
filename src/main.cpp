// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#include "platform.h"
#include "platform_def.h"

#include <cctype>
#include <cmath>
#include <cstdarg>

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_io.h"
#include "platform_i2c.h"
#include "platform_spi.h"
#include "platform_tone.h"

#else

#include <cstdio>
#include <cstdlib>
#include <cstring>

#endif

static constexpr int FLOAT_DECIMAL_PLACES = 5;
static constexpr int FLOAT_MIN_WIDTH = 4;

// Buffer for the current command line entered over Serial.
char line[MAX_LINE_LEN];

#ifdef ARDUINO
// Current index into the command line buffer (Arduino build only; the
// host build reads whole lines at once via fgets()).
static short lineIdx = 0;
#endif

extern unsigned char is_continue;
extern unsigned char running;

static void initMathBindings()
{
  platform_sin = std::sin;
  platform_cos = std::cos;
  platform_tan = std::tan;
  platform_acos = std::acos;
  platform_asin = std::asin;
  platform_atan = std::atan;

  platform_sinh = std::sinh;
  platform_cosh = std::cosh;
  platform_tanh = std::tanh;
  platform_asinh = std::asinh;
  platform_acosh = std::acosh;
  platform_atanh = std::atanh;

  platform_atan2 = std::atan2;

  platform_log = std::log;
  platform_log10 = std::log10;
  platform_exp = std::exp;
  platform_sqrt = std::sqrt;
  platform_floor = std::floor;
  platform_ceil = std::ceil;
}

#ifdef ARDUINO

// Print formatted text to the Serial interface.
int serialPrint(const char *format, ...)
{
  char buffer[MAX_LINE_LEN];
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  Serial.print(buffer);

  return len;
}

// Read an integer from Serial input, echoing characters back to the user.
int serialInput()
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

void delayMs(int ms)
{
  delay(ms);
}

#endif

int printError(const char *format, ...)
{
  int result;

  va_list args;
  va_start(args, format);

#ifdef ARDUINO
  char buffer[MAX_LINE_LEN];
  result = vsnprintf(buffer, sizeof(buffer), format, args);
  Serial.print(buffer);
#else
  result = vprintf(format, args);
#endif

  va_end(args);

  // If an error occurs, stop the running mode and enters into Immediate mode.
  running = 0;

  return result;
}

void printFloat(double num)
{
#ifdef ARDUINO
#if defined(ARDUINO_UNOR4_WIFI) || defined(ARDUINO_UNOWIFIR4)
  char floatStr[16];
  dtostrf(num, FLOAT_MIN_WIDTH, FLOAT_DECIMAL_PLACES, floatStr);

  if (strchr(floatStr, '.'))
  {
    // Trim trailing zeroes (and a trailing decimal point) produced by
    // dtostrf()'s fixed-precision formatting.
    for (int i = strlen(floatStr) - 1; i >= 0; i--)
    {
      if (floatStr[i] == '0')
      {
        floatStr[i] = '\0';
      }
      else if (floatStr[i] == '.')
      {
        floatStr[i] = '\0';
        break;
      }
      else
      {
        break;
      }
    }
  }

  Serial.print(floatStr);
#else
  serialPrint("%g", num);
#endif
#else
  printf("%g", num);
#endif
}

#ifdef ARDUINO

void setup()
{
  // Connect the parser's print callbacks to the Serial interface.
  err_print = printError;
  str_print = serialPrint;
  float_print = printFloat;
  warn_print = serialPrint;

  // Connect the platform abstraction callbacks.
  platform_delay_ms = delayMs;
  platform_analog_read = getAnalogPortValue;
  platform_pin_mode = setPinMode;
  platform_digital_write = setPinValue;
  platform_digital_read = getPinValue;
  platform_pwm = setPWM;
  platform_play_tone = playTone;
  int_input = serialInput;

  platform_is_key_pressed = isKeyPressed;

  platform_i2c_init = PlatformI2C::init;
  platform_i2c_start = PlatformI2C::start;
  platform_i2c_restart = PlatformI2C::restart;
  platform_i2c_stop = PlatformI2C::stop;
  platform_i2c_write = PlatformI2C::write;
  platform_i2c_read = PlatformI2C::read;

  platform_spi_init = spiInit;
  platform_spi_select = spiSelect;
  platform_spi_deselect = spiDeselect;
  platform_spi_transfer = spiTransfer;

  initMathBindings();

  lineIdx = 0;

  Serial.begin(SERIAL_BAUD_RATE);
  delay(500);

  init_parser();
  Serial.print("> ");
}

void loop()
{
  // Read available characters from the Serial interface and build a command line.
  while (Serial.available())
  {
    char c = Serial.read();

    if (c == '\n')
    {
      continue; // Ignore newline characters.
    }

    if (c == '\r')
    {
      // End of command line; terminate the string and parse it.
      line[lineIdx] = '\0';
      Serial.println();

      if (lineIdx > 0)
      {
        do_parse(line);
      }

      lineIdx = 0;
      Serial.print("> ");
    }
    else if ((c == '\b' || c == 127) && lineIdx > 0)
    {
      // Handle backspace/delete during command entry.
      lineIdx--;
      Serial.print("\b \b");
    }
    else if ((lineIdx < MAX_LINE_LEN - 1) && (c > 0x1F && c < 0x7F))
    {
      // Accept printable ASCII characters into the line buffer.
      line[lineIdx++] = c;
      Serial.print(c);
    }
  }
}

#else

int main()
{
  err_print = printError;
  str_print = printf;
  float_print = printFloat;
  warn_print = printf;

  initMathBindings();

  init_parser();

  while (is_continue)
  {
    printf("> ");
    fflush(stdout);

    if (!fgets(line, sizeof(line), stdin))
      break;

    // Strip trailing newline.
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
    {
      line[--len] = '\0';
    }

    // Skip blank input.
    char *p = line;
    while (*p == ' ' || *p == '\t')
      p++;

    if (*p == '\0')
      continue;

    do_parse(line);
  }

  printf("\r\n");

  return 0;
}

#endif