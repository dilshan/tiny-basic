#ifndef __PLATFORM_DEF_HEADER__
#define __PLATFORM_DEF_HEADER__

#define MAX_LINES 256
#define MAX_LINE_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

  extern int (*str_print)(const char*, ...);
  extern int (*err_print)(const char*, ...);
  extern int (*int_input)(void);

  extern void (*platform_delay_ms)(int ms);

  void init_parser(void);
  void do_parse(char *line);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_DEF_HEADER__ */