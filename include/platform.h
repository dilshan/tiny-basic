#ifndef __PLATFORM_DEF_HEADER__
#define __PLATFORM_DEF_HEADER__

#define SET_PULL_UP

#define MAX_LINES 256
#define MAX_LINE_LEN 256

#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

#define MAX_ANALOG_PIN_COUNT 16
#define MAX_DIGITAL_PIN_COUNT 54

#ifdef __cplusplus
extern "C" {
#endif

  extern int (*str_print)(const char*, ...);
  extern int (*err_print)(const char*, ...);
  extern int (*int_input)(void);

  extern void (*platform_delay_ms)(int ms);

  extern int (*platform_analog_read)(int pin);
  extern void (*platform_pin_mode)(int pin, int mode);
  extern void (*platform_digital_write)(int pin, int value);
  extern int (*platform_digital_read)(int pin);

  void init_parser(void);
  void do_parse(char *line);

  char *strcasestr(const char *haystack, const char *needle);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_DEF_HEADER__ */