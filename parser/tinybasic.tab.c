/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "./tinybasic.y"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "math.h"

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state* YY_BUFFER_STATE;
#endif

// Parser runtime limits.
#define STACK_DEPTH 32
#define LOOP_STACK_DEPTH 16

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
  int limit;
  int step;
  unsigned short ret_pc;
} LoopFrame;

extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

extern int yylex(void);
extern int yylineno;

// Variables A..Z stored as uppercase indices.
static int variables[26];

static Line program[MAX_LINES];
static LoopFrame loop_stack[LOOP_STACK_DEPTH];
static ConditionalJumpMap cjump_map[128];

static JumpType jump_pending = JUMP_NONE;
static short jump_target = 0;
static unsigned char if_skip = 0;

static unsigned short prog_size = 0;
static short pc = 0;
static unsigned char running = 0;
static size_t loop_top = 0;
static unsigned char cjump_map_size;

static short call_stack[STACK_DEPTH];
static short stack_top = 0;

unsigned char is_continue = 1;

void yyerror(const char* s);

static inline int var_get(char c) {
  return variables[toupper((unsigned char)c) - 'A'];
}
static inline void var_set(char c, int v) {
  variables[toupper((unsigned char)c) - 'A'] = v;
}

static int is_keyword_end(char c) {
    return c == '\0' || isspace((unsigned char)c);
}

// Detect the kind of a program line by examining its leading keyword.
// This classification is used to build jump targets for FOR/NEXT, WHILE/WEND,
// IF/ELSE/ENDIF, etc.
 static LineType get_prog_line_type(const char* line) {
    while (isspace((unsigned char)*line))
        line++;

    if (strncasecmp(line, "FOR", 3) == 0 && is_keyword_end(line[3]))
        return LINE_FOR;

    if (strncasecmp(line, "NEXT", 4) == 0 && is_keyword_end(line[4]))
        return LINE_NEXT;

    if (strncasecmp(line, "WHILE", 5) == 0 && is_keyword_end(line[5]))
        return LINE_WHILE;

    if (strncasecmp(line, "WEND", 4) == 0 && is_keyword_end(line[4]))
        return LINE_WEND;

    if (strncasecmp(line, "ELSE", 4) == 0 && is_keyword_end(line[4]))
        return LINE_ELSE;

    if (strncasecmp(line, "ENDIF", 5) == 0 && is_keyword_end(line[5]))
        return LINE_ENDIF;

    if (strncasecmp(line, "REPEAT", 6) == 0 && is_keyword_end(line[6]))
        return LINE_REPEAT;

    if (strncasecmp(line, "UNTIL", 5) == 0 && is_keyword_end(line[5]))
        return LINE_UNTIL;

    // Identify single line IF and multi-line IF statements.
    if (strncasecmp(line, "IF", 2) == 0 && is_keyword_end(line[2])){
        const char *then_pos = strcasestr(line, "THEN");
        if (then_pos != NULL && is_keyword_end(then_pos[4])) {
            then_pos += 4;

            while(isspace((unsigned char)*then_pos))
                then_pos++;
                
            return (*then_pos == '\0') ? LINE_IF : LINE_SINGLE_IF;
        } else {
            return LINE_OTHER;
        }
    }

    return LINE_OTHER;
}

// Store a numbered program line in the program buffer, keeping lines sorted.
static void prog_store(int num, const char* text) {
  int i;

  for (i = 0; i < prog_size; i++) {
    if (program[i].num == num) {
      strncpy(program[i].text, text, MAX_LINE_LEN - 1);
      program[i].text[MAX_LINE_LEN - 1] = '\0';
      program[i].type = get_prog_line_type(text);
      return;
    }

    if (program[i].num > num) {
      if (prog_size < MAX_LINES) {
        memmove(&program[i + 1], &program[i], (prog_size - i) * sizeof(Line));
        prog_size++;

        program[i].num = num;
        strncpy(program[i].text, text, MAX_LINE_LEN - 1);
        program[i].text[MAX_LINE_LEN - 1] = '\0';
        program[i].type = get_prog_line_type(text);
      }

      return;
    }
  }

  if (prog_size < MAX_LINES) {
    program[prog_size].num = num;
    strncpy(program[prog_size].text, text, MAX_LINE_LEN - 1);
    program[prog_size].text[MAX_LINE_LEN - 1] = '\0';
    program[prog_size].type = get_prog_line_type(text);

    prog_size++;
  }
}

static int prog_find(int num) {
  for (int i = 0; i < prog_size; i++)
    if (program[i].num == num) return i;

  return -1;
}

// Reset runtime state for a fresh program or after CLEAR.
static void flush_memory(void) {
    pc = 0;

    is_continue = 1;
    if_skip = 0;

    jump_pending = JUMP_NONE;
    jump_target = 0;
    cjump_map_size = 0;

    loop_top = 0;
    stack_top = 0;

    memset(variables, 0, sizeof(variables));
}

// Helper to avoid matching nested IF/ELSE/ENDIF entries when scanning for the
// matching line in the conditional jump map.
 static int is_line_already_exists(short num) {
    for(int i = 0; i < cjump_map_size; i++) {
        if((cjump_map[i].root_node_num == num) || (cjump_map[i].end_node_num == num) || (cjump_map[i].else_node_num == num)) {
            return 1;
        }
    }

    return 0;
}

static void build_conditional_jump_map(void) {
    LineType expected_root_node = LINE_OTHER;
    cjump_map_size = 0;

    for (int i = 0; i < prog_size; i++) {
        if(program[i].type != LINE_OTHER) {

            if((program[i].type == LINE_FOR) || (program[i].type == LINE_WHILE) || (program[i].type == LINE_REPEAT) || (program[i].type == LINE_IF)) {
                if (cjump_map_size >= (sizeof(cjump_map) / sizeof(cjump_map[0]))) {
                    err_print("Too many loop blocks\n");
                    return;
                }

                cjump_map[cjump_map_size].type = program[i].type;
                cjump_map[cjump_map_size].root_node_num = program[i].num;
                cjump_map[cjump_map_size].end_node_num = -1;
                cjump_map[cjump_map_size].else_node_num = -1;
                cjump_map_size++;
            }
            else
            {
                if(program[i].type == LINE_NEXT) expected_root_node = LINE_FOR;
                else if(program[i].type == LINE_WEND) expected_root_node = LINE_WHILE;  
                else if(program[i].type == LINE_UNTIL) expected_root_node = LINE_REPEAT; 
                else continue;

                for(int j = (cjump_map_size - 1); j >= 0; j--) {
                    if((cjump_map[j].type == expected_root_node) && (cjump_map[j].end_node_num == -1)) {
                        cjump_map[j].end_node_num = program[i].num;
                        break;
                    }
                }
            }
        }
    }

    // Map components of the IF conditions.
    for(int i = (cjump_map_size - 1); i >= 0; i--) {
        if((cjump_map[i].type == LINE_IF) && (cjump_map[i].end_node_num == -1)) {
            for(int j = (prog_find(cjump_map[i].root_node_num) + 1); j < prog_size; j++) {

                if(is_line_already_exists(program[j].num)) {
                    continue;
                }

                if(program[j].type == LINE_ELSE) {
                    cjump_map[i].else_node_num = program[j].num;
                    continue;
                }

                if(program[j].type == LINE_ENDIF) {
                    cjump_map[i].end_node_num = program[j].num;
                    break;
                }
            }
        }
    }

    // Looking for any missing nodes.
    for (int i = 0; i < cjump_map_size; i++) {
        if (cjump_map[i].end_node_num == -1) {
            err_print("Missing termination block for line %d\n", cjump_map[i].root_node_num);
        }
    }
}

static unsigned char find_lowest_loop_node(short current_node_num, ConditionalJumpMap **node) {   
    for(int i = cjump_map_size - 1; i >= 0; i--) {
        if((cjump_map[i].type == LINE_FOR) || (cjump_map[i].type == LINE_WHILE) || (cjump_map[i].type == LINE_REPEAT)) {
            if((cjump_map[i].root_node_num < current_node_num) && (cjump_map[i].end_node_num > current_node_num)) {
                *node = &cjump_map[i];
                return 1;
            }
        }
    }

    return 0;
}

static short find_end_node(short parent_node_num, LineType node_type) {
    for(int i = 0; i < cjump_map_size; i++) {
        if((cjump_map[i].type == node_type) && (cjump_map[i].root_node_num == parent_node_num)) {
            return cjump_map[i].end_node_num;
        }
    }

    return -1;
}

static short find_root_node(short end_node_num, LineType node_type) {
    for (int i = 0; i < cjump_map_size; i++) {
        if (cjump_map[i].type == node_type && cjump_map[i].end_node_num == end_node_num) {
            return cjump_map[i].root_node_num;
        }
    }

    return -1;
}

static short find_else_node(short parent_node_num) {
    for(int i = 0; i < cjump_map_size; i++) {
        if((cjump_map[i].type == LINE_IF) && (cjump_map[i].root_node_num == parent_node_num)) {
            return cjump_map[i].else_node_num;
        }
    }

    return -1;
}

static short find_end_node_from_else(short else_node) {
    for(int i = 0; i < cjump_map_size; i++) {
        if((cjump_map[i].type == LINE_IF) && (cjump_map[i].else_node_num == else_node)) {
            return cjump_map[i].end_node_num;
        }
    }

    return -1;
}

static void stack_push(short ret_pc) {
  if (stack_top >= STACK_DEPTH) {
    err_print("GOSUB stack overflow\n");
    return;
  }

  call_stack[stack_top++] = ret_pc;
}

static short stack_pop(void) {
  if (stack_top <= 0) {
    err_print("RETURN without GOSUB\n");
    return 0;
  }

  return call_stack[--stack_top];
}


#line 445 "tinybasic.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    VAR = 259,                     /* VAR  */
    STRING = 260,                  /* STRING  */
    PRINT = 261,                   /* PRINT  */
    IF = 262,                      /* IF  */
    THEN = 263,                    /* THEN  */
    ELSE = 264,                    /* ELSE  */
    ENDIF = 265,                   /* ENDIF  */
    GOTO = 266,                    /* GOTO  */
    INPUT = 267,                   /* INPUT  */
    LET = 268,                     /* LET  */
    GOSUB = 269,                   /* GOSUB  */
    RETURN = 270,                  /* RETURN  */
    CLEAR = 271,                   /* CLEAR  */
    LIST = 272,                    /* LIST  */
    RUN = 273,                     /* RUN  */
    END = 274,                     /* END  */
    CR = 275,                      /* CR  */
    NEW = 276,                     /* NEW  */
    RAND = 277,                    /* RAND  */
    FOR = 278,                     /* FOR  */
    TO = 279,                      /* TO  */
    STEP = 280,                    /* STEP  */
    NEXT = 281,                    /* NEXT  */
    DELAY = 282,                   /* DELAY  */
    ANALOG = 283,                  /* ANALOG  */
    HIGH = 284,                    /* HIGH  */
    LOW = 285,                     /* LOW  */
    PIN = 286,                     /* PIN  */
    IN = 287,                      /* IN  */
    OUT = 288,                     /* OUT  */
    GET = 289,                     /* GET  */
    SET = 290,                     /* SET  */
    ABS = 291,                     /* ABS  */
    REL_LT = 292,                  /* REL_LT  */
    REL_LE = 293,                  /* REL_LE  */
    REL_NE = 294,                  /* REL_NE  */
    REL_GT = 295,                  /* REL_GT  */
    REL_GE = 296,                  /* REL_GE  */
    WHILE = 297,                   /* WHILE  */
    WEND = 298,                    /* WEND  */
    EXIT = 299,                    /* EXIT  */
    REPEAT = 300,                  /* REPEAT  */
    UNTIL = 301,                   /* UNTIL  */
    MIN = 302,                     /* MIN  */
    MAX = 303,                     /* MAX  */
    BYTE = 304,                    /* BYTE  */
    HBYTE = 305,                   /* HBYTE  */
    LBYTE = 306,                   /* LBYTE  */
    LSHIFT = 307,                  /* LSHIFT  */
    RSHIFT = 308,                  /* RSHIFT  */
    MOD = 309,                     /* MOD  */
    WAIT = 310,                    /* WAIT  */
    SUM = 311,                     /* SUM  */
    SUMSQ = 312,                   /* SUMSQ  */
    POW = 313,                     /* POW  */
    AND = 314,                     /* AND  */
    OR = 315,                      /* OR  */
    UMINUS = 316,                  /* UMINUS  */
    UPLUS = 317,                   /* UPLUS  */
    INVERT = 318                   /* INVERT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 375 "./tinybasic.y"

    int   ival;     /* For numeric literals. */
    char  cval;     /* For single-letter variables. */
    char* sval;     /* For quoted strings. */

#line 561 "tinybasic.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMBER = 3,                     /* NUMBER  */
  YYSYMBOL_VAR = 4,                        /* VAR  */
  YYSYMBOL_STRING = 5,                     /* STRING  */
  YYSYMBOL_PRINT = 6,                      /* PRINT  */
  YYSYMBOL_IF = 7,                         /* IF  */
  YYSYMBOL_THEN = 8,                       /* THEN  */
  YYSYMBOL_ELSE = 9,                       /* ELSE  */
  YYSYMBOL_ENDIF = 10,                     /* ENDIF  */
  YYSYMBOL_GOTO = 11,                      /* GOTO  */
  YYSYMBOL_INPUT = 12,                     /* INPUT  */
  YYSYMBOL_LET = 13,                       /* LET  */
  YYSYMBOL_GOSUB = 14,                     /* GOSUB  */
  YYSYMBOL_RETURN = 15,                    /* RETURN  */
  YYSYMBOL_CLEAR = 16,                     /* CLEAR  */
  YYSYMBOL_LIST = 17,                      /* LIST  */
  YYSYMBOL_RUN = 18,                       /* RUN  */
  YYSYMBOL_END = 19,                       /* END  */
  YYSYMBOL_CR = 20,                        /* CR  */
  YYSYMBOL_NEW = 21,                       /* NEW  */
  YYSYMBOL_RAND = 22,                      /* RAND  */
  YYSYMBOL_FOR = 23,                       /* FOR  */
  YYSYMBOL_TO = 24,                        /* TO  */
  YYSYMBOL_STEP = 25,                      /* STEP  */
  YYSYMBOL_NEXT = 26,                      /* NEXT  */
  YYSYMBOL_DELAY = 27,                     /* DELAY  */
  YYSYMBOL_ANALOG = 28,                    /* ANALOG  */
  YYSYMBOL_HIGH = 29,                      /* HIGH  */
  YYSYMBOL_LOW = 30,                       /* LOW  */
  YYSYMBOL_PIN = 31,                       /* PIN  */
  YYSYMBOL_IN = 32,                        /* IN  */
  YYSYMBOL_OUT = 33,                       /* OUT  */
  YYSYMBOL_GET = 34,                       /* GET  */
  YYSYMBOL_SET = 35,                       /* SET  */
  YYSYMBOL_ABS = 36,                       /* ABS  */
  YYSYMBOL_REL_LT = 37,                    /* REL_LT  */
  YYSYMBOL_REL_LE = 38,                    /* REL_LE  */
  YYSYMBOL_REL_NE = 39,                    /* REL_NE  */
  YYSYMBOL_REL_GT = 40,                    /* REL_GT  */
  YYSYMBOL_REL_GE = 41,                    /* REL_GE  */
  YYSYMBOL_WHILE = 42,                     /* WHILE  */
  YYSYMBOL_WEND = 43,                      /* WEND  */
  YYSYMBOL_EXIT = 44,                      /* EXIT  */
  YYSYMBOL_REPEAT = 45,                    /* REPEAT  */
  YYSYMBOL_UNTIL = 46,                     /* UNTIL  */
  YYSYMBOL_MIN = 47,                       /* MIN  */
  YYSYMBOL_MAX = 48,                       /* MAX  */
  YYSYMBOL_BYTE = 49,                      /* BYTE  */
  YYSYMBOL_HBYTE = 50,                     /* HBYTE  */
  YYSYMBOL_LBYTE = 51,                     /* LBYTE  */
  YYSYMBOL_LSHIFT = 52,                    /* LSHIFT  */
  YYSYMBOL_RSHIFT = 53,                    /* RSHIFT  */
  YYSYMBOL_MOD = 54,                       /* MOD  */
  YYSYMBOL_WAIT = 55,                      /* WAIT  */
  YYSYMBOL_SUM = 56,                       /* SUM  */
  YYSYMBOL_SUMSQ = 57,                     /* SUMSQ  */
  YYSYMBOL_POW = 58,                       /* POW  */
  YYSYMBOL_AND = 59,                       /* AND  */
  YYSYMBOL_OR = 60,                        /* OR  */
  YYSYMBOL_61_ = 61,                       /* '+'  */
  YYSYMBOL_62_ = 62,                       /* '-'  */
  YYSYMBOL_63_ = 63,                       /* '*'  */
  YYSYMBOL_64_ = 64,                       /* '/'  */
  YYSYMBOL_UMINUS = 65,                    /* UMINUS  */
  YYSYMBOL_UPLUS = 66,                     /* UPLUS  */
  YYSYMBOL_INVERT = 67,                    /* INVERT  */
  YYSYMBOL_68_ = 68,                       /* '('  */
  YYSYMBOL_69_ = 69,                       /* ')'  */
  YYSYMBOL_70_ = 70,                       /* ','  */
  YYSYMBOL_71_ = 71,                       /* '='  */
  YYSYMBOL_YYACCEPT = 72,                  /* $accept  */
  YYSYMBOL_program = 73,                   /* program  */
  YYSYMBOL_line = 74,                      /* line  */
  YYSYMBOL_statement = 75,                 /* statement  */
  YYSYMBOL_76_1 = 76,                      /* $@1  */
  YYSYMBOL_expr_list = 77,                 /* expr_list  */
  YYSYMBOL_expr_item = 78,                 /* expr_item  */
  YYSYMBOL_var_list = 79,                  /* var_list  */
  YYSYMBOL_expression = 80,                /* expression  */
  YYSYMBOL_boolean_expr = 81,              /* boolean_expr  */
  YYSYMBOL_mode = 82,                      /* mode  */
  YYSYMBOL_sum_args = 83,                  /* sum_args  */
  YYSYMBOL_sumsq_args = 84,                /* sumsq_args  */
  YYSYMBOL_term = 85,                      /* term  */
  YYSYMBOL_factor = 86                     /* factor  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  72
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   397

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  15
/* YYNRULES -- Number of rules.  */
#define YYNRULES  84
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  207

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   318


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      68,    69,    63,    61,    70,    62,     2,    64,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    71,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    65,    66,    67
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   402,   402,   407,   408,   409,   413,   415,   425,   432,
     439,   451,   485,   524,   551,   566,   578,   587,   601,   627,
     626,   635,   659,   674,   685,   690,   692,   697,   702,   707,
     712,   717,   725,   727,   731,   732,   736,   750,   757,   767,
     780,   781,   782,   783,   784,   788,   792,   796,   800,   804,
     808,   812,   816,   821,   826,   833,   834,   838,   842,   849,
     853,   860,   861,   862,   871,   883,   884,   885,   886,   887,
     888,   889,   890,   891,   892,   893,   894,   895,   896,   897,
     898,   899,   900,   901,   902
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER", "VAR",
  "STRING", "PRINT", "IF", "THEN", "ELSE", "ENDIF", "GOTO", "INPUT", "LET",
  "GOSUB", "RETURN", "CLEAR", "LIST", "RUN", "END", "CR", "NEW", "RAND",
  "FOR", "TO", "STEP", "NEXT", "DELAY", "ANALOG", "HIGH", "LOW", "PIN",
  "IN", "OUT", "GET", "SET", "ABS", "REL_LT", "REL_LE", "REL_NE", "REL_GT",
  "REL_GE", "WHILE", "WEND", "EXIT", "REPEAT", "UNTIL", "MIN", "MAX",
  "BYTE", "HBYTE", "LBYTE", "LSHIFT", "RSHIFT", "MOD", "WAIT", "SUM",
  "SUMSQ", "POW", "AND", "OR", "'+'", "'-'", "'*'", "'/'", "UMINUS",
  "UPLUS", "INVERT", "'('", "')'", "','", "'='", "$accept", "program",
  "line", "statement", "$@1", "expr_list", "expr_item", "var_list",
  "expression", "boolean_expr", "mode", "sum_args", "sumsq_args", "term",
  "factor", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-63)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-22)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     342,     1,   -62,   197,   -63,   -63,   238,   -59,    17,   238,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,    27,    31,   -45,
     -19,   -16,   197,   -63,   -63,   -63,   197,    -4,    70,   -63,
      55,    71,   156,   -63,   -63,    43,    44,   -63,   -63,    45,
      64,    67,    75,    96,   101,   103,   119,   123,   125,   128,
     129,   279,   279,   130,   197,    80,   146,   -13,   -63,   238,
     -24,   206,    62,   -24,   140,   -63,   238,   238,   238,   -63,
     -63,   238,   -63,   -63,   -63,   -63,    -1,   -63,   -24,   147,
     279,   279,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   -13,   -13,   279,    65,   153,   238,   238,
     238,   238,   238,   238,   238,   279,   279,   238,   195,   279,
     279,   279,    46,   -63,     2,   238,   238,    86,   -28,    -7,
      61,   -63,   156,   -63,   163,   165,   114,    76,    83,   120,
     133,   159,    88,   104,   -24,     4,   -24,    18,   118,   169,
     -63,   -63,   -24,   -24,   -24,   -24,   -24,   -24,   -24,   -13,
     -13,   -24,     1,   -63,   -63,   -63,   -63,   235,   -24,   -22,
     -63,    57,   238,   238,   -63,   -63,   -63,   -63,   238,   238,
     -63,   -63,   -63,   238,   238,   -63,   238,   -63,   238,   238,
     -63,   -63,   -63,   238,   -63,   -63,   171,   168,   174,   200,
     209,   215,   241,   -24,   -24,   250,     5,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   238,   -24
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,    22,    23,     0,     0,     0,     0,
      28,    30,    31,    32,    33,     5,    29,     0,     0,     0,
       0,     0,     0,    15,    18,    16,     0,     0,     0,     2,
       0,     0,     0,    66,    65,     0,     0,    71,    72,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    46,     0,    40,    61,     0,
      24,     0,     0,    27,     0,    13,     0,     0,     0,    14,
      17,     0,     1,     4,     3,    36,     0,    34,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    19,     0,
       0,     0,     0,    38,     0,     0,     0,     0,     0,     0,
       0,     6,     0,    73,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    57,     0,    59,     0,     0,     0,
      67,    45,    49,    50,    48,    51,    52,    53,    54,    43,
      44,    47,     0,    64,    62,    63,    25,     0,    26,     0,
       7,     0,     0,     0,    35,    68,    69,    74,     0,     0,
      77,    78,    79,     0,     0,    83,     0,    82,     0,     0,
      70,    20,    39,     0,    55,    56,     0,     0,     0,     0,
       0,     0,     0,    58,    60,     0,    11,     8,     9,    10,
      75,    76,    80,    81,    84,     0,    12
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -63,   -63,   -63,     0,   -63,   -63,   134,   -63,    -6,     3,
     -63,   -63,   -63,   -47,    19
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    28,    29,    30,   152,    76,    77,   114,    55,    56,
     186,   135,   137,    57,    58
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      60,    31,   183,    63,    93,    94,    32,     2,     3,    61,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    62,    16,    66,    17,    69,    78,    18,    19,    70,
     205,    64,    20,   105,   106,    65,    21,   105,   106,   105,
     106,   109,   161,    22,    23,    24,    25,    26,    96,    67,
     110,   111,    68,   112,   105,   106,    27,    97,   149,   150,
     117,   118,   119,   162,    71,   120,   105,   106,   121,   122,
      72,   156,   157,   175,   176,    73,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   136,   138,   177,   178,   184,
     185,    74,   142,   143,   144,   145,   146,   147,   148,   124,
     125,   151,    98,    99,   100,   101,   102,   105,   106,   158,
     159,    79,    80,    81,   139,   140,    78,    98,    99,   100,
     101,   102,   105,   106,   103,   104,   105,   106,   153,   154,
     155,   163,    82,   115,   140,    83,   107,   105,   106,   103,
     104,   105,   106,    84,   105,   106,   168,   105,   106,   105,
     106,   107,   181,   169,   108,   160,   187,   188,   173,    33,
      34,    75,   189,   190,    85,   105,   106,   191,   192,    86,
     193,    87,   194,   195,   174,   105,   106,   196,    35,   105,
     106,   105,   106,   167,    36,    37,    38,    88,   179,   170,
      39,    89,    40,    90,   105,   106,    91,    92,    95,   206,
      33,    34,   171,    41,    42,    43,    44,    45,    46,    47,
     113,   116,    48,    49,    50,   -21,   123,    51,    52,    35,
     105,   106,   141,    53,    59,    36,    37,    38,   172,   105,
     106,    39,   165,    40,   166,   105,   106,   198,   180,   182,
     197,    33,    34,   199,    41,    42,    43,    44,    45,    46,
      47,     0,     0,    48,    49,    50,   164,     0,    51,    52,
      35,   105,   106,     0,    53,    54,    36,    37,    38,   200,
     105,   106,    39,     0,    40,     0,   105,   106,   201,     0,
       0,     0,    33,    34,   202,    41,    42,    43,    44,    45,
      46,    47,     0,     0,    48,    49,    50,     0,     0,    51,
      52,    35,   105,   106,     0,    53,    59,    36,    37,    38,
     203,   105,   106,    39,     0,    40,     0,     0,     0,   204,
       0,     0,     0,     0,     0,     0,    41,    42,    43,    44,
      45,    46,    47,     0,     0,    48,    49,    50,     0,     0,
       0,     0,     0,     0,     0,     1,    53,    59,     2,     3,
       0,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,     0,    17,     0,     0,    18,    19,
       0,     0,     0,    20,     0,     0,     0,    21,     0,     0,
       0,     0,     0,     0,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,    27
};

static const yytype_int16 yycheck[] =
{
       6,     1,    24,     9,    51,    52,    68,     6,     7,    68,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     4,    21,    68,    23,    22,    32,    26,    27,    26,
      25,     4,    31,    61,    62,     4,    35,    61,    62,    61,
      62,    54,    70,    42,    43,    44,    45,    46,    54,    68,
      63,    64,    68,    59,    61,    62,    55,    54,   105,   106,
      66,    67,    68,    70,    68,    71,    61,    62,    69,    70,
       0,    69,    70,    69,    70,    20,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    69,    70,    32,
      33,    20,    98,    99,   100,   101,   102,   103,   104,    80,
      81,   107,    37,    38,    39,    40,    41,    61,    62,   115,
     116,    68,    68,    68,    95,    69,   122,    37,    38,    39,
      40,    41,    61,    62,    59,    60,    61,    62,   109,   110,
     111,    70,    68,    71,    69,    68,    71,    61,    62,    59,
      60,    61,    62,    68,    61,    62,    70,    61,    62,    61,
      62,    71,   152,    70,     8,    69,   162,   163,    70,     3,
       4,     5,   168,   169,    68,    61,    62,   173,   174,    68,
     176,    68,   178,   179,    70,    61,    62,   183,    22,    61,
      62,    61,    62,    69,    28,    29,    30,    68,    70,    69,
      34,    68,    36,    68,    61,    62,    68,    68,    68,   205,
       3,     4,    69,    47,    48,    49,    50,    51,    52,    53,
       4,    71,    56,    57,    58,    20,    69,    61,    62,    22,
      61,    62,    69,    67,    68,    28,    29,    30,    69,    61,
      62,    34,    69,    36,    69,    61,    62,    69,    69,     4,
      69,     3,     4,    69,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    56,    57,    58,   122,    -1,    61,    62,
      22,    61,    62,    -1,    67,    68,    28,    29,    30,    69,
      61,    62,    34,    -1,    36,    -1,    61,    62,    69,    -1,
      -1,    -1,     3,     4,    69,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    56,    57,    58,    -1,    -1,    61,
      62,    22,    61,    62,    -1,    67,    68,    28,    29,    30,
      69,    61,    62,    34,    -1,    36,    -1,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    56,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    67,    68,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    23,    -1,    -1,    26,    27,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    43,    44,    45,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     6,     7,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    23,    26,    27,
      31,    35,    42,    43,    44,    45,    46,    55,    73,    74,
      75,    75,    68,     3,     4,    22,    28,    29,    30,    34,
      36,    47,    48,    49,    50,    51,    52,    53,    56,    57,
      58,    61,    62,    67,    68,    80,    81,    85,    86,    68,
      80,    68,     4,    80,     4,     4,    68,    68,    68,    81,
      81,    68,     0,    20,    20,     5,    77,    78,    80,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    85,    85,    68,    80,    81,    37,    38,
      39,    40,    41,    59,    60,    61,    62,    71,     8,    54,
      63,    64,    80,     4,    79,    71,    71,    80,    80,    80,
      80,    69,    70,    69,    86,    86,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    83,    80,    84,    80,    86,
      69,    69,    80,    80,    80,    80,    80,    80,    80,    85,
      85,    80,    76,    86,    86,    86,    69,    70,    80,    80,
      69,    70,    70,    70,    78,    69,    69,    69,    70,    70,
      69,    69,    69,    70,    70,    69,    70,    69,    70,    70,
      69,    75,     4,    24,    32,    33,    82,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    69,    69,    69,
      69,    69,    69,    69,    69,    25,    80
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    72,    73,    74,    74,    74,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    76,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    77,    77,    78,    78,    79,    79,
      80,    80,    80,    80,    80,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    82,    82,    83,    83,    84,
      84,    85,    85,    85,    85,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     2,     1,     4,     4,     6,     6,
       6,     6,     8,     2,     2,     1,     1,     2,     1,     0,
       5,     3,     1,     1,     2,     4,     4,     2,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     3,
       1,     2,     2,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     3,     1,
       3,     1,     3,     3,     3,     1,     1,     3,     4,     4,
       4,     1,     1,     3,     4,     6,     6,     4,     4,     4,
       6,     6,     4,     4,     6
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 6: /* statement: PRINT '(' expr_list ')'  */
#line 413 "./tinybasic.y"
                                       { if (!if_skip) str_print("\n"); }
#line 1792 "tinybasic.tab.c"
    break;

  case 7: /* statement: DELAY '(' expression ')'  */
#line 416 "./tinybasic.y"
        {
            if (!if_skip) {
                int ms = (yyvsp[-1].ival);
                if (ms < 0) ms = 0;
                
                platform_delay_ms(ms);
            }
        }
#line 1805 "tinybasic.tab.c"
    break;

  case 8: /* statement: PIN '(' expression ',' mode ')'  */
#line 426 "./tinybasic.y"
        {
            if (!if_skip) {
                platform_pin_mode((yyvsp[-3].ival), (yyvsp[-1].ival));
            }
        }
#line 1815 "tinybasic.tab.c"
    break;

  case 9: /* statement: SET '(' expression ',' expression ')'  */
#line 433 "./tinybasic.y"
        {
            if (!if_skip) {
                platform_digital_write((yyvsp[-3].ival), (yyvsp[-1].ival));
            }
        }
#line 1825 "tinybasic.tab.c"
    break;

  case 10: /* statement: WAIT '(' expression ',' expression ')'  */
#line 440 "./tinybasic.y"
        {
            if (!if_skip) {
                while(is_continue) {
                    if(platform_digital_read((yyvsp[-3].ival)) == (yyvsp[-1].ival)) {
                        break;
                    }                    
                    platform_delay_ms(10);
                }
            }
        }
#line 1840 "tinybasic.tab.c"
    break;

  case 11: /* statement: FOR VAR '=' expression TO expression  */
#line 452 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    int start = (yyvsp[-2].ival);
                    int limit = (yyvsp[0].ival);
                    int step  = 1;

                    var_set((yyvsp[-4].cval), start);

                    if ((step > 0) ? (start > limit) : (start < limit)) {
                        jump_target = find_end_node(program[pc].num, program[pc].type);
                        if (jump_target < 0) {
                            err_print("Missing matching NEXT\n");
                        }
                        else {
                            jump_pending = JUMP_CONDITION_SKIP;
                        }
                    }
                    else {
                        loop_stack[loop_top].type = LOOP_FOR;
                        loop_stack[loop_top].var = toupper((yyvsp[-4].cval));
                        loop_stack[loop_top].limit = limit;
                        loop_stack[loop_top].step = step;
                        loop_stack[loop_top].ret_pc = pc + 1;

                        loop_top++;
                    }
                }
            }
        }
#line 1877 "tinybasic.tab.c"
    break;

  case 12: /* statement: FOR VAR '=' expression TO expression STEP expression  */
#line 486 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    int start = (yyvsp[-4].ival);
                    int limit = (yyvsp[-2].ival);
                    int step  = (yyvsp[0].ival);

                    if (step == 0) {
                        err_print("STEP cannot be zero\n");
                    }
                    else {
                        var_set((yyvsp[-6].cval), start);

                        if ((step > 0) ? (start > limit) : (start < limit)) {
                            jump_target = find_end_node(program[pc].num, program[pc].type);
                            if (jump_target < 0) {
                                err_print("Missing matching NEXT\n");
                            }
                            else {
                                jump_pending = JUMP_CONDITION_SKIP;
                            }
                        }
                        else {
                            loop_stack[loop_top].type = LOOP_FOR;
                            loop_stack[loop_top].var = toupper((yyvsp[-6].cval));
                            loop_stack[loop_top].limit = limit;
                            loop_stack[loop_top].step = step;
                            loop_stack[loop_top].ret_pc = pc + 1;

                            loop_top++;
                        }
                    }
                }
            }
        }
#line 1919 "tinybasic.tab.c"
    break;

  case 13: /* statement: NEXT VAR  */
#line 525 "./tinybasic.y"
       {
            if ((running) && (!if_skip)) {
                if (loop_top == 0) {
                    err_print("NEXT without matching FOR\n");
                }
                else if (loop_stack[loop_top - 1].type != LOOP_FOR) {
                    err_print("NEXT without matching FOR\n");
                }
                else if (loop_stack[loop_top - 1].var != toupper((yyvsp[0].cval))) {
                    err_print("Mismatched NEXT variable\n");
                } else {
                    LoopFrame* f = &loop_stack[loop_top - 1];
                    int newval = var_get(f->var) + f->step;
                    var_set(f->var, newval);

                    int done = (f->step > 0) ? (newval > f->limit) : (newval < f->limit);
                    if (!done) {
                      jump_pending = JUMP_GOTO;
                      jump_target = program[f->ret_pc].num;
                    } else {
                      loop_top--;
                    }
                }
            }
       }
#line 1949 "tinybasic.tab.c"
    break;

  case 14: /* statement: WHILE boolean_expr  */
#line 552 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                if (!(yyvsp[0].ival)) {

                    jump_target = find_end_node(program[pc].num, LINE_WHILE);

                    if (jump_target < 0)
                        err_print("Missing WEND\n");
                    else
                        jump_pending = JUMP_CONDITION_SKIP;
                }
            }
        }
#line 1967 "tinybasic.tab.c"
    break;

  case 15: /* statement: WEND  */
#line 567 "./tinybasic.y"
        {
           if ((running) && (!if_skip)) {
                jump_target = find_root_node(program[pc].num, LINE_WHILE);

                if (jump_target < 0)
                    err_print("WEND without WHILE\n");
                else
                    jump_pending = JUMP_GOTO;
            }
        }
#line 1982 "tinybasic.tab.c"
    break;

  case 16: /* statement: REPEAT  */
#line 579 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                if(find_end_node(program[pc].num, LINE_REPEAT) < 0) {
                    err_print("Missing UNTIL\n");
                }
            }
        }
#line 1994 "tinybasic.tab.c"
    break;

  case 17: /* statement: UNTIL boolean_expr  */
#line 588 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                if (!(yyvsp[0].ival)) {
                    jump_target = find_root_node(program[pc].num, LINE_REPEAT);

                    if (jump_target < 0)
                        err_print("Missing REPEAT\n");
                    else
                        jump_pending = JUMP_CONDITION_SKIP;
                }
            }
        }
#line 2011 "tinybasic.tab.c"
    break;

  case 18: /* statement: EXIT  */
#line 602 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                ConditionalJumpMap* lowest_loop_node = NULL;

                if(find_lowest_loop_node(program[pc].num, &lowest_loop_node)) {
                    jump_target = lowest_loop_node->end_node_num;

                    if((lowest_loop_node->type == LINE_FOR) && (loop_top > 0)) {
                        loop_top--;
                    }

                    if (jump_target < 0) {
                        err_print("EXIT without a loop\n");
                    }
                    else {
                        jump_pending = JUMP_CONDITION_SKIP;
                    }
                }
                else {
                    err_print("EXIT without a loop\n");
                }
            }
        }
#line 2039 "tinybasic.tab.c"
    break;

  case 19: /* $@1: %empty  */
#line 627 "./tinybasic.y"
        {
            // Only evaluate the condition if we're not already skipping due to an outer IF.
            if (!if_skip) {
                if_skip = !(yyvsp[-1].ival);
            }
        }
#line 2050 "tinybasic.tab.c"
    break;

  case 21: /* statement: IF boolean_expr THEN  */
#line 636 "./tinybasic.y"
        {
            if ((running) && (!if_skip)) {
                if(!(yyvsp[-1].ival))
                {
                    short else_node = find_else_node(program[pc].num);

                    if(else_node >= 0) {
                        jump_target = else_node;
                    }
                    else {
                        jump_target = find_end_node(program[pc].num, LINE_IF);
                    }

                    if(jump_target < 0) {
                        err_print("Missing ENDIF\n");
                    }
                    else {
                        jump_pending = JUMP_CONDITION_SKIP;
                    }
                }
            }
        }
#line 2077 "tinybasic.tab.c"
    break;

  case 22: /* statement: ELSE  */
#line 660 "./tinybasic.y"
        {
            if((running) && (!if_skip))
            {
                jump_target = find_end_node_from_else(program[pc].num);

                if(jump_target < 0) {
                    err_print("Missing ENDIF\n");
                }
                else {
                    jump_pending = JUMP_CONDITION_SKIP;
                }
            }
        }
#line 2095 "tinybasic.tab.c"
    break;

  case 23: /* statement: ENDIF  */
#line 675 "./tinybasic.y"
        {
            if((running) && (!if_skip)) {
                if(find_root_node(program[pc].num, LINE_IF) == -1) {
                    if(program[pc].type != LINE_SINGLE_IF) {
                        err_print("ENDIF without IF\n");
                    }
                }
            }
        }
#line 2109 "tinybasic.tab.c"
    break;

  case 24: /* statement: GOTO expression  */
#line 686 "./tinybasic.y"
        {
            if (!if_skip) { jump_pending = JUMP_GOTO; jump_target = (yyvsp[0].ival); }
        }
#line 2117 "tinybasic.tab.c"
    break;

  case 26: /* statement: LET VAR '=' expression  */
#line 693 "./tinybasic.y"
        {
            if (!if_skip) var_set((yyvsp[-2].cval), (yyvsp[0].ival));
        }
#line 2125 "tinybasic.tab.c"
    break;

  case 27: /* statement: GOSUB expression  */
#line 698 "./tinybasic.y"
        {
            if (!if_skip) { jump_pending = JUMP_GOSUB; jump_target = (yyvsp[0].ival); }
        }
#line 2133 "tinybasic.tab.c"
    break;

  case 28: /* statement: RETURN  */
#line 703 "./tinybasic.y"
        {
            if (!if_skip) { pc = stack_pop(); jump_pending = JUMP_RETURN; }
        }
#line 2141 "tinybasic.tab.c"
    break;

  case 29: /* statement: NEW  */
#line 708 "./tinybasic.y"
        {
            if (!if_skip) { prog_size = 0; flush_memory(); }
        }
#line 2149 "tinybasic.tab.c"
    break;

  case 30: /* statement: CLEAR  */
#line 713 "./tinybasic.y"
        {
            if (!if_skip) flush_memory(); 
        }
#line 2157 "tinybasic.tab.c"
    break;

  case 31: /* statement: LIST  */
#line 718 "./tinybasic.y"
        {
            if (!if_skip) {
                for (int i = 0; i < prog_size; i++)
                    str_print("%d %s\n", program[i].num, program[i].text);
            }
        }
#line 2168 "tinybasic.tab.c"
    break;

  case 32: /* statement: RUN  */
#line 725 "./tinybasic.y"
            { if (!if_skip) { if(!running) { running = 1; pc = 0; } } }
#line 2174 "tinybasic.tab.c"
    break;

  case 33: /* statement: END  */
#line 727 "./tinybasic.y"
            { if (!if_skip) { if(!running) is_continue = 0; else running = 0; } }
#line 2180 "tinybasic.tab.c"
    break;

  case 36: /* expr_item: STRING  */
#line 737 "./tinybasic.y"
        {
            if (!if_skip) {
                char* s = (yyvsp[0].sval);
                int len = strlen(s);

                // Strip surrounding quotes.
                if (len >= 2) {
                    s[len - 1] = '\0';
                    str_print("%s", s + 1);
                }
                free((yyvsp[0].sval));
            }
        }
#line 2198 "tinybasic.tab.c"
    break;

  case 37: /* expr_item: expression  */
#line 751 "./tinybasic.y"
        {
            if (!if_skip) str_print("%d", (yyvsp[0].ival));
        }
#line 2206 "tinybasic.tab.c"
    break;

  case 38: /* var_list: VAR  */
#line 758 "./tinybasic.y"
        {
            if (!if_skip) {
                int v;
                str_print("? ");
                
                v = int_input();
                var_set((yyvsp[0].cval), v);
            }
        }
#line 2220 "tinybasic.tab.c"
    break;

  case 39: /* var_list: var_list ',' VAR  */
#line 768 "./tinybasic.y"
        {
           if (!if_skip) {
                int v;
                str_print("? ");
                
                v = int_input();
                var_set((yyvsp[0].cval), v);
            }
        }
#line 2234 "tinybasic.tab.c"
    break;

  case 40: /* expression: term  */
#line 780 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[0].ival); }
#line 2240 "tinybasic.tab.c"
    break;

  case 41: /* expression: '+' term  */
#line 781 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[0].ival); }
#line 2246 "tinybasic.tab.c"
    break;

  case 42: /* expression: '-' term  */
#line 782 "./tinybasic.y"
                                 { (yyval.ival) = -(yyvsp[0].ival); }
#line 2252 "tinybasic.tab.c"
    break;

  case 43: /* expression: expression '+' term  */
#line 783 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-2].ival) + (yyvsp[0].ival); }
#line 2258 "tinybasic.tab.c"
    break;

  case 44: /* expression: expression '-' term  */
#line 784 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-2].ival) - (yyvsp[0].ival); }
#line 2264 "tinybasic.tab.c"
    break;

  case 45: /* boolean_expr: '(' boolean_expr ')'  */
#line 789 "./tinybasic.y"
        {
            (yyval.ival) = (yyvsp[-1].ival);
        }
#line 2272 "tinybasic.tab.c"
    break;

  case 46: /* boolean_expr: expression  */
#line 793 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[0].ival) != 0);
        }
#line 2280 "tinybasic.tab.c"
    break;

  case 47: /* boolean_expr: expression '=' expression  */
#line 797 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) == (yyvsp[0].ival));
        }
#line 2288 "tinybasic.tab.c"
    break;

  case 48: /* boolean_expr: expression REL_NE expression  */
#line 801 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) != (yyvsp[0].ival));
        }
#line 2296 "tinybasic.tab.c"
    break;

  case 49: /* boolean_expr: expression REL_LT expression  */
#line 805 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) < (yyvsp[0].ival));
        }
#line 2304 "tinybasic.tab.c"
    break;

  case 50: /* boolean_expr: expression REL_LE expression  */
#line 809 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) <= (yyvsp[0].ival));
        }
#line 2312 "tinybasic.tab.c"
    break;

  case 51: /* boolean_expr: expression REL_GT expression  */
#line 813 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) > (yyvsp[0].ival));
        }
#line 2320 "tinybasic.tab.c"
    break;

  case 52: /* boolean_expr: expression REL_GE expression  */
#line 817 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) >= (yyvsp[0].ival));
        }
#line 2328 "tinybasic.tab.c"
    break;

  case 53: /* boolean_expr: expression AND expression  */
#line 822 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) && (yyvsp[0].ival));
        }
#line 2336 "tinybasic.tab.c"
    break;

  case 54: /* boolean_expr: expression OR expression  */
#line 827 "./tinybasic.y"
        {
            (yyval.ival) = ((yyvsp[-2].ival) || (yyvsp[0].ival)); 
        }
#line 2344 "tinybasic.tab.c"
    break;

  case 55: /* mode: IN  */
#line 833 "./tinybasic.y"
                                 { (yyval.ival) = PIN_MODE_INPUT;  }
#line 2350 "tinybasic.tab.c"
    break;

  case 56: /* mode: OUT  */
#line 834 "./tinybasic.y"
                                 { (yyval.ival) = PIN_MODE_OUTPUT; }
#line 2356 "tinybasic.tab.c"
    break;

  case 57: /* sum_args: expression  */
#line 839 "./tinybasic.y"
       {
            (yyval.ival) = (yyvsp[0].ival); 
       }
#line 2364 "tinybasic.tab.c"
    break;

  case 58: /* sum_args: sum_args ',' expression  */
#line 843 "./tinybasic.y"
       {
            (yyval.ival) = (yyvsp[-2].ival) + (yyvsp[0].ival);
       }
#line 2372 "tinybasic.tab.c"
    break;

  case 59: /* sumsq_args: expression  */
#line 850 "./tinybasic.y"
       {
            (yyval.ival) = ((yyvsp[0].ival) * (yyvsp[0].ival)); 
       }
#line 2380 "tinybasic.tab.c"
    break;

  case 60: /* sumsq_args: sumsq_args ',' expression  */
#line 854 "./tinybasic.y"
       {
            (yyval.ival) = (yyvsp[-2].ival) + ((yyvsp[0].ival) * (yyvsp[0].ival));
       }
#line 2388 "tinybasic.tab.c"
    break;

  case 61: /* term: factor  */
#line 860 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[0].ival); }
#line 2394 "tinybasic.tab.c"
    break;

  case 62: /* term: term '*' factor  */
#line 861 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-2].ival) * (yyvsp[0].ival); }
#line 2400 "tinybasic.tab.c"
    break;

  case 63: /* term: term '/' factor  */
#line 863 "./tinybasic.y"
        {
            if ((yyvsp[0].ival) == 0) { 
                err_print("Division by zero\n"); 
                (yyval.ival) = 0; 
            }
            else          
                (yyval.ival) = (yyvsp[-2].ival) / (yyvsp[0].ival);
        }
#line 2413 "tinybasic.tab.c"
    break;

  case 64: /* term: term MOD factor  */
#line 872 "./tinybasic.y"
        {
             if ((yyvsp[0].ival) == 0) { 
                err_print("Division by zero\n"); 
                (yyval.ival) = 0; 
            }
            else          
                (yyval.ival) = (yyvsp[-2].ival) % (yyvsp[0].ival);
        }
#line 2426 "tinybasic.tab.c"
    break;

  case 65: /* factor: VAR  */
#line 883 "./tinybasic.y"
                                 { (yyval.ival) = var_get((yyvsp[0].cval)); }
#line 2432 "tinybasic.tab.c"
    break;

  case 66: /* factor: NUMBER  */
#line 884 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[0].ival); }
#line 2438 "tinybasic.tab.c"
    break;

  case 67: /* factor: '(' expression ')'  */
#line 885 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-1].ival); }
#line 2444 "tinybasic.tab.c"
    break;

  case 68: /* factor: ANALOG '(' factor ')'  */
#line 886 "./tinybasic.y"
                                 { (yyval.ival) = platform_analog_read((yyvsp[-1].ival)); }
#line 2450 "tinybasic.tab.c"
    break;

  case 69: /* factor: GET '(' factor ')'  */
#line 887 "./tinybasic.y"
                                 { (yyval.ival) = platform_digital_read((yyvsp[-1].ival)); }
#line 2456 "tinybasic.tab.c"
    break;

  case 70: /* factor: INVERT '(' factor ')'  */
#line 888 "./tinybasic.y"
                                 { (yyval.ival) = !((yyvsp[-1].ival)); }
#line 2462 "tinybasic.tab.c"
    break;

  case 71: /* factor: HIGH  */
#line 889 "./tinybasic.y"
                                 { (yyval.ival) = 1; }
#line 2468 "tinybasic.tab.c"
    break;

  case 72: /* factor: LOW  */
#line 890 "./tinybasic.y"
                                 { (yyval.ival) = 0; }
#line 2474 "tinybasic.tab.c"
    break;

  case 73: /* factor: RAND '(' ')'  */
#line 891 "./tinybasic.y"
                                 { (yyval.ival) = rand() % 32768; }
#line 2480 "tinybasic.tab.c"
    break;

  case 74: /* factor: ABS '(' expression ')'  */
#line 892 "./tinybasic.y"
                                 { (yyval.ival) = abs((yyvsp[-1].ival)); }
#line 2486 "tinybasic.tab.c"
    break;

  case 75: /* factor: MIN '(' expression ',' expression ')'  */
#line 893 "./tinybasic.y"
                                                    { (yyval.ival) = min((yyvsp[-3].ival), (yyvsp[-1].ival)); }
#line 2492 "tinybasic.tab.c"
    break;

  case 76: /* factor: MAX '(' expression ',' expression ')'  */
#line 894 "./tinybasic.y"
                                                    { (yyval.ival) = max((yyvsp[-3].ival), (yyvsp[-1].ival)); }
#line 2498 "tinybasic.tab.c"
    break;

  case 77: /* factor: BYTE '(' expression ')'  */
#line 895 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-1].ival) & 0xFF; }
#line 2504 "tinybasic.tab.c"
    break;

  case 78: /* factor: HBYTE '(' expression ')'  */
#line 896 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-1].ival) & 0xF0; }
#line 2510 "tinybasic.tab.c"
    break;

  case 79: /* factor: LBYTE '(' expression ')'  */
#line 897 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-1].ival) & 0x0F; }
#line 2516 "tinybasic.tab.c"
    break;

  case 80: /* factor: LSHIFT '(' expression ',' expression ')'  */
#line 898 "./tinybasic.y"
                                                    { (yyval.ival) = (yyvsp[-3].ival) << (yyvsp[-1].ival); }
#line 2522 "tinybasic.tab.c"
    break;

  case 81: /* factor: RSHIFT '(' expression ',' expression ')'  */
#line 899 "./tinybasic.y"
                                                    { (yyval.ival) = (yyvsp[-3].ival) >> (yyvsp[-1].ival); }
#line 2528 "tinybasic.tab.c"
    break;

  case 82: /* factor: SUMSQ '(' sumsq_args ')'  */
#line 900 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-1].ival); }
#line 2534 "tinybasic.tab.c"
    break;

  case 83: /* factor: SUM '(' sum_args ')'  */
#line 901 "./tinybasic.y"
                                 { (yyval.ival) = (yyvsp[-1].ival); }
#line 2540 "tinybasic.tab.c"
    break;

  case 84: /* factor: POW '(' expression ',' expression ')'  */
#line 902 "./tinybasic.y"
                                                    { (yyval.ival) = power((yyvsp[-3].ival), (yyvsp[-1].ival)); }
#line 2546 "tinybasic.tab.c"
    break;


#line 2550 "tinybasic.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 905 "./tinybasic.y"


void yyerror(const char* s) { err_print("Error: %s\n", s); }

static void do_run(void) {
  jump_pending = JUMP_NONE;
  
  stack_top = 0;
  loop_top = 0;
  
  cjump_map_size = 0;
  pc = 0;

  build_conditional_jump_map();

#ifdef DEBUG
  printf("Conditional Jump Map\n");
  for(int i = 0; i < cjump_map_size; i++) {
      printf("Type=%d, Root=%d, End=%d, Else=%d\n", cjump_map[i].type, cjump_map[i].root_node_num, cjump_map[i].end_node_num, cjump_map[i].else_node_num);
  }
  printf("--------------------------------\n");
#endif

  while (running && pc < prog_size) {
    char exec[MAX_LINE_LEN + 2];
    snprintf(exec, sizeof(exec), "%s\n", program[pc].text);

    jump_pending = JUMP_NONE;
    if_skip = 0;

    YY_BUFFER_STATE buf = yy_scan_string(exec);
    yyparse();
    yy_delete_buffer(buf);

    if (!running) break;

    switch (jump_pending) {
      case JUMP_GOTO: {
        int idx = prog_find(jump_target);
        if (idx < 0) {
          err_print("Undefined line %d\n", jump_target);
          goto done;
        }

        pc = idx;
        break;
      }

      case JUMP_GOSUB: {
        int idx = prog_find(jump_target);
        if (idx < 0) {
          err_print("Undefined line %d\n", jump_target);
          goto done;
        }

        stack_push(pc + 1);
        pc = idx;
        break;
      }

      case JUMP_CONDITION_SKIP: {
        int idx = prog_find(jump_target);
        if (idx < 0) {
          err_print("Missing conditional boundary\n");
          goto done;
        }

        pc = idx + 1;
        break;
      }

      case JUMP_RETURN:
        break;

      default:
        pc++;
        break;
    }
  }

done:
  running = 0;
}

void init_parser(void) {
  memset(variables, 0, sizeof(variables));
  str_print("Tiny BASIC \n");
}

void do_parse(char *line) {
  int len = strlen(line);
  if (len > 0 && line[len - 1] == '\n') {
    line[--len] = '\0';
  }

  // skip blank input.
  char* p = line;
  while (*p == ' ' || *p == '\t') p++;

  if (*p == '\0') return;

  if (isdigit((unsigned char)*p)) {
    // Numbered line: store it, don't execute.
    int num = 0;
    while (isdigit((unsigned char)*p)) num = num * 10 + (*p++ - '0');

    while (*p == ' ' || *p == '\t') p++;

    prog_store(num, p);
  } else {
    // Immediate mode: parse and execute.
    char with_nl[MAX_LINE_LEN + 2];

    snprintf(with_nl, sizeof(with_nl), "%s\n", line);

    running = 0;
    jump_pending = JUMP_NONE;
    if_skip = 0;

    YY_BUFFER_STATE buf = yy_scan_string(with_nl);
    yyparse();
    yy_delete_buffer(buf);

    if (running) do_run();
  }
}
