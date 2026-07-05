// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_def.h"
#include "platform_sys.h"

void platformSys::delayMs(int ms)
{
  delay(ms);
}

#endif