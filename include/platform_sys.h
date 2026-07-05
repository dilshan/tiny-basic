// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_SYS_HEADER__
#define __PLATFORM_SYS_HEADER__

class platformSys {
  public:
    platformSys() = delete;

    static void delayMs(int ms);
};

#endif /* __PLATFORM_SYS_HEADER__ */