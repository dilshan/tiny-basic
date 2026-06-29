#include <stddef.h>
#include "platform.h"

int (*str_print)(const char*, ...) = NULL;
int (*err_print)(const char*, ...) = NULL;
int (*int_input)(void) = NULL;

void (*platform_delay_ms)(int ms) = NULL;

int (*platform_analog_read)(int pin) = NULL;
void (*platform_pin_mode)(int pin, int mode) = NULL;
void (*platform_digital_write)(int pin, int value) = NULL;
int (*platform_digital_read)(int pin) = NULL;

int (*is_key_pressed)(void) = NULL;