// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#ifndef __PLATFORM_PARSER_HLP_HEADER__
#define __PLATFORM_PARSER_HLP_HEADER__

#include "platform_def.h"

// Macro for arithmetic operators (+, -, *, /).
#define DO_MATH_OP(res, a, b, op) \
    if ((a).type == VAL_FLOAT || (b).type == VAL_FLOAT) { \
        (res) = make_float(to_float(a) op to_float(b)); \
    } else { \
        (res) = make_int((a).as.i op (b).as.i); \
    }

// Macro for relational operators (=, <, >).
#define DO_REL_OP(res, a, b, op) \
    if ((a).type == VAL_FLOAT || (b).type == VAL_FLOAT) { \
        (res) = make_int(to_float(a) op to_float(b)); \
    } else { \
        (res) = make_int((a).as.i op (b).as.i); \
    }

// Macro to ensure that a value is an integer before performing an operation.
#define REQUIRE_INT(res, d, f) \
    if((d).type != VAL_INT) { \
        err_print(ERR_REQUIRED_INT);\
        (res) = make_int(0); \
    } else { \
        (res) = make_int(f); \
    }

// Macro to ensure that two values are integers before performing an operation.
#define REQUIRE_INT_EX(res, d1, d2, f) \
    if(((d1).type != VAL_INT) || ((d2).type != VAL_INT)){ \
        err_print(ERR_REQUIRED_INT);\
        (res) = make_int(0); \
    } else { \
        (res) = make_int(f); \
    }

// Control flow actions that can be requested during parsing.
typedef enum {
    JUMP_NONE,
    JUMP_GOTO,
    JUMP_GOSUB,
    JUMP_RETURN,
    JUMP_CONDITION_SKIP
} JumpType;

// Supported loop constructs.
typedef enum {
    LOOP_FOR,
    LOOP_WHILE
} LoopType;

// Line categories used to build jump targets for structured flow.
typedef enum {
    LINE_OTHER = 0,
    LINE_FOR,
    LINE_NEXT,
    LINE_WHILE,
    LINE_WEND,
    LINE_SINGLE_IF,
    LINE_IF,
    LINE_ELSE,
    LINE_ENDIF,
    LINE_UNTIL,
    LINE_REPEAT
} LineType;

// Print mode for integers.
typedef enum {
    PRINT_DEC,
    PRINT_HEX,
    PRINT_BIN,
    PRINT_BIN_GRP,
    PRINT_OCT
} PrintMode;

// Mapping from control structure root lines to matching terminators.
typedef struct {
    LineType type;
    short root_node_num;
    short end_node_num;
    short else_node_num;
} ConditionalJumpMap;

// Represents a stored program line in numbered program mode.
typedef struct {
  short num;
  LineType type;
  char text[MAX_LINE_LEN];
} Line;

// Frame used for FOR/NEXT loop state.
typedef struct {
  LoopType type;
  char var;
  double limit;
  double step;
  unsigned short ret_pc;
} LoopFrame;

typedef struct {
    unsigned char index;
    unsigned char target;
    short target_num;
} OnGotoFrame;

#endif /* __PLATFORM_PARSER_HLP_HEADER__ */