#include "platform.h"
#include "platform_def.h"

#include <cmath>

// DEBUG mode is used for testing the parser on a host machine without Arduino hardware.
// To use the DEBUG mode, run the make command from the root directory.

#ifndef DEBUG

#include <Arduino.h>

#include "platform_io.h"
#include "platform_i2c.h"
#include "platform_spi.h"

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 256
#define MAX_LINE_LEN 256

#endif

// Buffer for the current command line entered over Serial.
char line[MAX_LINE_LEN];
// Current index into the command line buffer.
short lineIdx = 0;

extern unsigned char is_continue;
extern unsigned char running;

#ifndef DEBUG
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
      if ((idx < sizeof(numBuffer) - 1) && (isdigit(c) || (c == '-' && idx == 0)))
      {
        numBuffer[idx++] = c;
        Serial.print(c);
      }
    }
  }
}

void delayMs(int ms)
{
  delay(ms);
}

void setup()
{
  // Connect the parser's print callbacks to the Serial interface.
  str_print = serialPrint;
  err_print = serialPrint;
  warn_print = serialPrint;

  // Connect the platform abstraction callbacks.
  platform_delay_ms = delayMs;
  platform_analog_read = getAnalogPortValue;
  platform_pin_mode = setPinMode;
  platform_digital_write = setPinValue;
  platform_digital_read = getPinValue;
  int_input = serialInput;

  platform_is_key_pressed = isKeyPressed;

  platform_i2c_init = i2cInit;
  platform_i2c_start = i2cStart;
  platform_i2c_restart = i2cRestart;
  platform_i2c_stop = i2cStop;
  platform_i2c_write = i2cWrite;
  platform_i2c_read = i2cRead;

  platform_spi_init = spiInit;
  platform_spi_select = spiSelect;
  platform_spi_deselect = spiDeselect;
  platform_spi_transfer = spiTransfer;

  // Connect the math function pointers to the standard library functions.
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
  err_print = printf;
  str_print = printf;
  warn_print = printf;

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

  init_parser();

  while (is_continue)
  {
    printf("> ");
    fflush(stdout);

    if (!fgets(line, sizeof(line), stdin))
      break;

    // strip trailing newline.
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
    {
      line[--len] = '\0';
    }

    // skip blank input.
    char *p = line;
    while (*p == ' ' || *p == '\t')
      p++;

    if (*p == '\0')
      continue;

    do_parse(line);
  }

  printf("\n");

  return 0;
}

#endif