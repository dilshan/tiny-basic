#ifndef __PLATFORM_DEF_HEADER__
#define __PLATFORM_DEF_HEADER__

#define INT_SIZE  32

// Maximum number of stored command lines and characters per line.
#define MAX_LINES 256
#define MAX_LINE_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

// Function pointers provided by the platform implementation.
extern int (*str_print)(const char*, ...);
extern int (*err_print)(const char*, ...);
extern int (*warn_print)(const char*, ...);
extern int (*int_input)(void);

extern void (*platform_delay_ms)(int ms);

extern int (*platform_analog_read)(int pin);
extern void (*platform_pin_mode)(int pin, int mode);
extern void (*platform_digital_write)(int pin, int value);
extern int (*platform_digital_read)(int pin);

extern int (*platform_is_key_pressed)(void);

extern void (*platform_i2c_init)(void);
extern void (*platform_i2c_start)(void);
extern void (*platform_i2c_restart)(void);
extern unsigned char (*platform_i2c_write)(unsigned char data);
extern unsigned char (*platform_i2c_read)(unsigned char ack);
extern void (*platform_i2c_stop)(void);

extern void (*platform_spi_init)(void);
extern void (*platform_spi_select)(void);
extern void (*platform_spi_deselect)(void);
extern unsigned char (*platform_spi_transfer)(unsigned char output_byte);

extern unsigned char platform_spi_read_buffer;

extern double (*platform_sin)(double num);
extern double (*platform_cos)(double num);
extern double (*platform_tan)(double num);
extern double (*platform_asin)(double num);
extern double (*platform_acos)(double num);
extern double (*platform_atan)(double num);

extern double (*platform_sinh)(double num);
extern double (*platform_cosh)(double num);
extern double (*platform_tanh)(double num);
extern double (*platform_asinh)(double num);
extern double (*platform_acosh)(double num);
extern double (*platform_atanh)(double num);

extern double (*platform_atan2)(double y, double x);

extern double (*platform_log)(double num);
extern double (*platform_log10)(double num);
extern double (*platform_exp)(double num);
extern double (*platform_sqrt)(double num);
extern double (*platform_floor)(double num);
extern double (*platform_ceil)(double num);

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