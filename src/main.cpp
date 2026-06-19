#include <Arduino.h>

#include "platform.h"

char line[MAX_LINE_LEN];
int lineIdx = 0;

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

      if (idx < sizeof(numBuffer) - 1)
      {
        numBuffer[idx++] = c;
        Serial.print(c);
      }
    }
  }
}

void setup()
{
  str_print = serialPrint;
  err_print = serialPrint;
  int_input = serialInput;

  Serial.begin(9600);
  delay(1000);

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

      do_parse(line);

      lineIdx = 0;
      Serial.print("> ");
    }
    else if ((c == '\b' || c == 127) && lineIdx > 0)
    {
      lineIdx--;
      Serial.print("\b \b");
    }
    else if (lineIdx < MAX_LINE_LEN - 1)
    {
      line[lineIdx++] = c;
      Serial.print(c);
    }
  }
}
