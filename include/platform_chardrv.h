// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_CHARDRV_HEADER__
#define __PLATFORM_CHARDRV_HEADER__

class PlatformCharDrv {
  public:
    PlatformCharDrv() = delete;
    
    static int print(const char *format, ...);
    static int input();
    static int isKeyPressed();
};

#endif /* __PLATFORM_CHARDRV_HEADER__ */