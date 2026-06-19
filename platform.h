#ifndef __PLATFORM_DEF_HEADER__
#define __PLATFORM_DEF_HEADER__

extern int (*str_print)(const char*, ...);
extern int (*err_print)(const char*, ...);
extern int (*int_input)(void);

#endif /* __PLATFORM_DEF_HEADER__ */