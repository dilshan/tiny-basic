#ifndef _TINY_BASIC_PLATFORM_DEF_
#define _TINY_BASIC_PLATFORM_DEF_

#define PPRINTF(...)     printf(__VA_ARGS__)
#define PPRINT_ERR(...)  fprintf(stderr, __VA_ARGS__)

#define PSCANF(...)      scanf(__VA_ARGS__)

#define RANDOM()         rand()

#endif /* _TINY_BASIC_PLATFORM_DEF_ */