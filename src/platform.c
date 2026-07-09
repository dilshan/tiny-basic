// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

/**
 * @brief Defines the platform abstraction interface for Tiny Basic.
 * @details This file wires the runtime to platform-specific callback hooks for
 *          console I/O, timing, pin access, I2C/SPI, audio, and math operations.
 */

#include <stddef.h>
#include "platform.h"

int (*str_print)(const char*, ...) = NULL;
void (*chr_print)(char c) = NULL;
void (*float_print)(double value) = NULL;
int (*err_print)(const char*, ...) = NULL;
int (*warn_print)(const char*, ...) = NULL;
int (*int_input)(void) = NULL;

void (*platform_delay_ms)(int ms) = NULL;

int (*platform_analog_read)(int pin) = NULL;
void (*platform_pin_mode)(int pin, int mode) = NULL;
void (*platform_digital_write)(int pin, int value) = NULL;
int (*platform_digital_read)(int pin) = NULL;
void (*platform_pwm)(int pin, unsigned char value) = NULL;

void (*platform_play_tone)(int freq, int duration) = NULL;

int (*platform_is_key_pressed)(void) = NULL;

void (*platform_i2c_init)(void) = NULL;
void (*platform_i2c_start)(void) = NULL;
void (*platform_i2c_restart)(void) = NULL;
unsigned char (*platform_i2c_write)(unsigned char data) = NULL;
unsigned char (*platform_i2c_read)(unsigned char ack) = NULL;
void (*platform_i2c_stop)(void) = NULL;

unsigned char platform_spi_read_buffer = 0xFF;

void (*platform_spi_init)(void) = NULL;
void (*platform_spi_select)(void) = NULL;
void (*platform_spi_deselect)(void) = NULL;
unsigned char (*platform_spi_transfer)(unsigned char output_byte) = NULL;

double (*platform_sin)(double num) = NULL;
double (*platform_cos)(double num) = NULL;
double (*platform_tan)(double num) = NULL;
double (*platform_asin)(double num) = NULL;
double (*platform_acos)(double num) = NULL;
double (*platform_atan)(double num) = NULL;

double (*platform_sinh)(double num) = NULL;
double (*platform_cosh)(double num) = NULL;
double (*platform_tanh)(double num) = NULL;
double (*platform_asinh)(double num) = NULL;
double (*platform_acosh)(double num) = NULL;
double (*platform_atanh)(double num) = NULL;

double (*platform_atan2)(double y, double x) = NULL;

double (*platform_log)(double num) = NULL;
double (*platform_log10)(double num) = NULL;    
double (*platform_exp)(double num) = NULL;
double (*platform_sqrt)(double num) = NULL;
double (*platform_floor)(double num) = NULL;
double (*platform_ceil)(double num) = NULL;