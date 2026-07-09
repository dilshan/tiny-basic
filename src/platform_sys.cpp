// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_def.h"
#include "platform_sys.h"

/**
 * @brief Delays the program execution for a specified number of milliseconds.
 * @param ms The number of milliseconds to delay.
 */
void platformSys::delayMs(int ms)
{
  delay(ms);
}

#endif