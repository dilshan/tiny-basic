// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_SPI_HEADER__
#define __PLATFORM_SPI_HEADER__

class PlatformSPI {
  public:
    PlatformSPI() = delete;

    static void init();
    static void select();
    static void deselect();

    static unsigned char transfer(unsigned char output_byte);
};

#endif /* __PLATFORM_SPI_HEADER__ */