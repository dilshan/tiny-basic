#ifndef __PLATFORM_VALUE_HEADER__
#define __PLATFORM_VALUE_HEADER__

// Variable value types.
typedef enum { 
    VAL_INT, 
    VAL_FLOAT, 
    VAL_VAR
} ValType;

// Data structure for tagged value system.
typedef struct {
    ValType type;
    union {
        int i;
        double f;
        char* s;
    } as;
} VarVal;

#endif /* __PLATFORM_VALUE_HEADER__ */