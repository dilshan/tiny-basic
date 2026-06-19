#include <stddef.h>
#include "platform.h"

int (*str_print)(const char*, ...) = NULL;
int (*err_print)(const char*, ...) = NULL;
int (*int_input)(void) = NULL;