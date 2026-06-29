#ifndef __PLATFORM_DEF_HEADER__
#define __PLATFORM_DEF_HEADER__

#define INT_SIZE  32

// Maximum number of stored command lines and characters per line.
#define MAX_LINES 256
#define MAX_LINE_LEN 256

// Digital pin modes for the platform abstraction.
#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

// Maximum supported analog and digital pins for the target board.
#define MAX_ANALOG_PIN_COUNT 16
#define MAX_DIGITAL_PIN_COUNT 54

// Enable internal use of pull-up resistors when available.
// If enabled, the SET operation always activate the internal pull-ups of the MCU digital inputs.
#define SET_PULL_UP

#ifdef __cplusplus
extern "C" {
#endif

// Function pointers provided by the platform implementation.
extern int (*str_print)(const char*, ...);
extern int (*err_print)(const char*, ...);
extern int (*int_input)(void);

extern void (*platform_delay_ms)(int ms);

extern int (*platform_analog_read)(int pin);
extern void (*platform_pin_mode)(int pin, int mode);
extern void (*platform_digital_write)(int pin, int value);
extern int (*platform_digital_read)(int pin);

extern int (*is_key_pressed)(void);

void init_parser(void);
void do_parse(char *line);

#ifndef DEBUG

// Declrations to remove compiler warninigs.
char *strcasestr(const char *haystack, const char *needle);

#endif /* DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_DEF_HEADER__ */