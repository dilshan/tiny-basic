// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_I2C_HEADER__
#define __PLATFORM_I2C_HEADER__

class PlatformI2C {
  public:
    PlatformI2C() = delete;
    
    static void init();
    static void start();
    static void restart();
    static void stop();

    static unsigned char write(unsigned char data);
    static unsigned char read(unsigned char send_ack);
};

#endif /* __PLATFORM_I2C_HEADER__ */