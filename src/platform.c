#include <stddef.h>
#include "platform.h"

int (*str_print)(const char*, ...) = NULL;
int (*err_print)(const char*, ...) = NULL;
int (*warn_print)(const char*, ...) = NULL;
int (*int_input)(void) = NULL;

void (*platform_delay_ms)(int ms) = NULL;

int (*platform_analog_read)(int pin) = NULL;
void (*platform_pin_mode)(int pin, int mode) = NULL;
void (*platform_digital_write)(int pin, int value) = NULL;
int (*platform_digital_read)(int pin) = NULL;

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