// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_spi.h"
#include "platform_def.h"

// Set to 250 kHz clock speed.
#define SPI_DELAY() delayMicroseconds(2)

void PlatformSPI::init() {
    pinMode(SPI_MOSI_PIN, OUTPUT);
    pinMode(SPI_MISO_PIN, INPUT);
    pinMode(SPI_SCK_PIN, OUTPUT);
    pinMode(SPI_CS_PIN, OUTPUT);
    
    digitalWrite(SPI_SCK_PIN, LOW);
    digitalWrite(SPI_CS_PIN, HIGH);
    digitalWrite(SPI_MOSI_PIN, LOW);
}

void PlatformSPI::select() {
    digitalWrite(SPI_CS_PIN, LOW);
    SPI_DELAY();
}

void PlatformSPI::deselect() {
    digitalWrite(SPI_CS_PIN, HIGH);
    SPI_DELAY();
}

unsigned char PlatformSPI::transfer(unsigned char output_byte) {
    unsigned char input_byte = 0;
    
    for (int i = 0; i < 8; i++) {
        if (output_byte & 0x80) {
            digitalWrite(SPI_MOSI_PIN, HIGH);
        } else {
            digitalWrite(SPI_MOSI_PIN, LOW);
        }
        output_byte <<= 1;
        SPI_DELAY();
        
        digitalWrite(SPI_SCK_PIN, HIGH);
        SPI_DELAY();
        
        input_byte <<= 1;
        if (digitalRead(SPI_MISO_PIN) == HIGH) {
            input_byte |= 0x01;
        }
        
        digitalWrite(SPI_SCK_PIN, LOW);
    }
    
    return input_byte;
}

#undef SPI_DELAY

#endif