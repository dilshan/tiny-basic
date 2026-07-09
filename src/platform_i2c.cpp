// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifdef ARDUINO

#include <Arduino.h>

#include "platform_i2c.h"
#include "platform_def.h"

// Set to 100kHz standard mode speed.
#define I2C_DELAY() delayMicroseconds(4)

/**
 * @brief Initializes the I2C bus by configuring the SDA and SCL pins as inputs.
 * @note This function should be called before any other I2C operations.
 */
void PlatformI2C::init () {
    pinMode(I2C_SDA_PIN, INPUT);
    pinMode(I2C_SCL_PIN, INPUT);
}

/**
 * @brief Generates an I2C start condition on the bus.
 */
void PlatformI2C::start() {
    pinMode(I2C_SDA_PIN, OUTPUT); digitalWrite(I2C_SDA_PIN, LOW);
    I2C_DELAY();
    pinMode(I2C_SCL_PIN, OUTPUT); digitalWrite(I2C_SCL_PIN, LOW);
    I2C_DELAY();
}

/**
 * @brief Generates an I2C restart condition on the bus.
 * @note This function is typically used when the master wants to initiate a new
 *       communication sequence without releasing the bus.
 */
void PlatformI2C::restart() {
    pinMode(I2C_SDA_PIN, INPUT);
    I2C_DELAY();
    pinMode(I2C_SCL_PIN, INPUT);
    I2C_DELAY();
    start();
}

/**
 * @brief Generates an I2C stop condition on the bus.
 * @note This function should be called after completing a communication sequence
 *       to release the bus for other devices.
 */
unsigned char PlatformI2C::write(unsigned char data) {
    // Clock out 8-bits.
    for (int i = 0; i < 8; i++) {
        if (data & 0x80) {
            pinMode(I2C_SDA_PIN, INPUT);
        } else {
            pinMode(I2C_SDA_PIN, OUTPUT); digitalWrite(I2C_SDA_PIN, LOW);
        }
        data <<= 1;
        I2C_DELAY();
        pinMode(I2C_SCL_PIN, INPUT);
        I2C_DELAY();
        pinMode(I2C_SCL_PIN, OUTPUT); digitalWrite(I2C_SCL_PIN, LOW);
        I2C_DELAY();
    }
    
    // Read 9th bit - ACK/NACK from the slave.
    pinMode(I2C_SDA_PIN, INPUT); 
    I2C_DELAY();
    pinMode(I2C_SCL_PIN, INPUT);
    I2C_DELAY();

    unsigned char ack = (digitalRead(I2C_SDA_PIN) == LOW);
    pinMode(I2C_SCL_PIN, OUTPUT); digitalWrite(I2C_SCL_PIN, LOW);
    I2C_DELAY();
    return ack; 
}

/**
 * @brief Reads a byte of data from the I2C bus and sends an ACK or NACK signal.
 * @param send_ack If true, sends an ACK signal after reading; otherwise, sends a
 * NACK signal.
 * @return The byte of data read from the I2C bus.
 */
unsigned char PlatformI2C::read(unsigned char send_ack) {
    unsigned char data = 0;
    pinMode(I2C_SDA_PIN, INPUT);
    
    for (int i = 0; i < 8; i++) {
        I2C_DELAY();
        pinMode(I2C_SCL_PIN, INPUT);
        I2C_DELAY();
        data <<= 1;
        if (digitalRead(I2C_SDA_PIN) == HIGH) data |= 0x01;
        pinMode(I2C_SCL_PIN, OUTPUT); digitalWrite(I2C_SCL_PIN, LOW);
    }
    
    // Send ACK or NACK master signal.
    if (send_ack) {
        pinMode(I2C_SDA_PIN, OUTPUT); digitalWrite(I2C_SDA_PIN, LOW);
    } else {
        pinMode(I2C_SDA_PIN, INPUT);
    }

    I2C_DELAY();
    pinMode(I2C_SCL_PIN, INPUT);
    I2C_DELAY();
    pinMode(I2C_SCL_PIN, OUTPUT); digitalWrite(I2C_SCL_PIN, LOW);
    pinMode(I2C_SDA_PIN, INPUT);
    I2C_DELAY();
    return data;
}

/**
 * @brief Generates an I2C stop condition on the bus.
 * @note This function should be called after completing a communication sequence
 *       to release the bus for other devices.
 */
void PlatformI2C::stop() {
    pinMode(I2C_SDA_PIN, OUTPUT); digitalWrite(I2C_SDA_PIN, LOW);
    I2C_DELAY();
    pinMode(I2C_SCL_PIN, INPUT);
    I2C_DELAY();
    pinMode(I2C_SDA_PIN, INPUT);
    I2C_DELAY();
}

#undef I2C_DELAY

#endif