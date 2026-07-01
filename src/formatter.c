// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#include <stdio.h>
#include <stdlib.h>

void int_to_binary(int num, char *buffer, unsigned char is_split)
{
  if (buffer == NULL)
  {
    return;
  }

  if (num == 0)
  {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
  }

  unsigned int u_num = (unsigned int)num;
  size_t idx = 0;
  int started = 0;

  for (int i = 31; i >= 0; i--)
  {
    int bit = (u_num >> i) & 1;

    if (bit == 1)
    {
      started = 1;
    }

    if (started)
    {
      if (is_split && idx > 0 && ((i + 1) % 8 == 0))
      {
        buffer[idx++] = ' ';
      }

      buffer[idx++] = bit ? '1' : '0';
    }
  }

  buffer[idx] = '\0';
}