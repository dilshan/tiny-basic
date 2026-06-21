#include <Arduino.h>

#include "platform.h"
#include "platform_io.h"

char line[MAX_LINE_LEN];
short lineIdx = 0;

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
        continue;

      if (c == '\r')
      {
        numBuffer[idx] = '\0';
        Serial.println();
        return atoi(numBuffer);
      }

      if ((c == '\b' || c == 127) && idx > 0)
      {
        idx--;
        Serial.print("\b \b");
        continue;
      }

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
  str_print = serialPrint;
  err_print = serialPrint;

  platform_delay_ms = delayMs;
  
  platform_analog_read = getAnalogPortValue;
  
  platform_pin_mode = setPinMode;
  platform_digital_write = setPinValue;
  platform_digital_read = getPinValue;
  int_input = serialInput;
  
  lineIdx = 0;

  Serial.begin(9600);
  delay(500);

  init_parser();
  Serial.print("> ");
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();

    if (c == '\n')
    {
      continue;
    }

    if (c == '\r')
    {
      line[lineIdx] = '\0';
      Serial.println();

      if(lineIdx > 0)
      {
        do_parse(line);
      }

      lineIdx = 0;
      Serial.print("> ");
    }
    else if ((c == '\b' || c == 127) && lineIdx > 0)
    {
      lineIdx--;
      Serial.print("\b \b");
    }
    else if ((lineIdx < MAX_LINE_LEN - 1) && (c > 0x1F && c < 0x7F))
    {
      line[lineIdx++] = c;
      Serial.print(c);
    }
  }
}
