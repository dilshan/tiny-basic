// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_IO_HEADER__
#define __PLATFORM_IO_HEADER__

class platformIO {
  public:
    platformIO() = delete;

    static int getAnalogPortValue(int pin);
    static void setPWM(int pin, unsigned char value);

    static void setPinMode(int pin, int mode);
    static void setPinValue(int pin, int value);
    static int getPinValue(int pin);

    static int isKeyPressed();
};

#endif /* __PLATFORM_IO_HEADER__ */