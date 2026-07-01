// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_IO_HEADER__
#define __PLATFORM_IO_HEADER__

int getAnalogPortValue(int pin);

void setPinMode(int pin, int mode);
void setPinValue(int pin, int value);
int getPinValue(int pin);

int isKeyPressed();

#endif /* __PLATFORM_IO_HEADER__ */