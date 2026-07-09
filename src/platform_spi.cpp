// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_spi.h"
#include "platform_def.h"

// Set to 250 kHz clock speed.
#define SPI_DELAY() delayMicroseconds(2)

/**
 * @brief Initializes the SPI bus by configuring the MOSI, MISO, SCK, and CS pins.
 * @note This function should be called before any other SPI operations.
 */
void PlatformSPI::init() {
    pinMode(SPI_MOSI_PIN, OUTPUT);
    pinMode(SPI_MISO_PIN, INPUT);
    pinMode(SPI_SCK_PIN, OUTPUT);
    pinMode(SPI_CS_PIN, OUTPUT);
    
    digitalWrite(SPI_SCK_PIN, LOW);
    digitalWrite(SPI_CS_PIN, HIGH);
    digitalWrite(SPI_MOSI_PIN, LOW);
}

/**
 * @brief Selects the SPI slave device by pulling the CS pin low.
 * @note This function should be called before starting a communication sequence with the slave device.
 */
void PlatformSPI::select() {
    digitalWrite(SPI_CS_PIN, LOW);
    SPI_DELAY();
}

/**
 * @brief Deselects the SPI slave device by pulling the CS pin high.
 * @note This function should be called after completing a communication sequence with the slave device.
 */
void PlatformSPI::deselect() {
    digitalWrite(SPI_CS_PIN, HIGH);
    SPI_DELAY();
}

/**
 * @brief Transfers a byte of data over the SPI bus and simultaneously receives a byte from the slave device.
 * @param output_byte The byte of data to send to the slave device.
 * @return The byte of data received from the slave device.
 * @note This function performs a full-duplex transfer, meaning that while sending data to the slave, it also reads data from the slave. The received byte is returned after the transfer is complete
 */
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