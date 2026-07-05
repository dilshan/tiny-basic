#ifdef ARDUINO

#include <Arduino.h>

#include "platform_def.h"
#include "platform_sys.h"

void platformSys::delayMs(int ms)
{
  delay(ms);
}

#endif