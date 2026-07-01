%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"
#include "platform.h"
#include "platform_def.h"
#include "math.h"
#include "formatter.h"

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state* YY_BUFFER_STATE;
#endif

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

#define REQUIRE_INT(res, d, f) \
    if((d).type != VAL_INT) { \
        err_print("Required integer type\n");\
        (res) = make_int(0); \
    } else { \
        (res) = make_int(f); \
    }

#define REQUIRE_INT_EX(res, d1, d2, f) \
    if(((d1).type != VAL_INT) || ((d2).type != VAL_INT)){ \
        err_print("Required integer type\n");\
        (res) = make_int(0); \
    } else { \
        (res) = make_int(f); \
    }

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

extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

extern int yylex(void);
extern int yylineno;

// Variables A..Z stored as uppercase indices.
static VarVal variables[26];

static Line program[MAX_LINES];
static LoopFrame loop_stack[LOOP_STACK_DEPTH];
static ConditionalJumpMap cjump_map[128];

static JumpType jump_pending = JUMP_NONE;
static short jump_target = 0;
static unsigned char if_skip = 0;

static unsigned short prog_size = 0;
static short pc = 0;
static size_t loop_top = 0;
static unsigned char cjump_map_size;
static OnGotoFrame on_goto_frame;

static short call_stack[STACK_DEPTH];
static short stack_top = 0;

static PrintMode current_print_mode = PRINT_DEC;

unsigned char is_continue = 1;
unsigned char running = 0;

void yyerror(const char* s);

static inline VarVal make_int(int i) { 
    VarVal v; 
    v.type = VAL_INT; 
    v.as.i = i; 
    return v; 
}

static inline VarVal make_float(double f) { 
    VarVal v; 
    v.type = VAL_FLOAT; 
    v.as.f = f; 
    return v; 
}

static inline int to_int(VarVal v) {
    if (v.type == VAL_INT) return v.as.i;
    if (v.type == VAL_FLOAT) return (int)v.as.f;
    return 0;
}

static inline double to_float(VarVal v) {
    if (v.type == VAL_FLOAT) return v.as.f;
    if (v.type == VAL_INT) return (double)v.as.i;
    return 0.0;
}

static inline VarVal var_get(char c) {
    return variables[toupper((unsigned char)c) - 'A'];
}
static inline void var_set(char c, VarVal v) {
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
    
    current_print_mode = PRINT_DEC;

    platform_spi_read_buffer = 0xFF;

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

%}

%union {
    int    ival;
    double fval;
    char   cval;
    char*  sval;
    VarVal val;
}

// Terminals for BASIC keywords and operators.
%token <ival> NUMBER
%token <fval> FLOAT_NUMBER
%token <cval> VAR
%token <sval> STRING

%token PRINT IF THEN ELSE ENDIF GOTO INPUT LET GOSUB RETURN CLEAR LIST RUN END CR
%token NEW RAND FOR TO STEP NEXT DELAY ANALOG HIGH LOW PIN IN OUT GET SET ABS
%token REL_LT REL_LE REL_NE REL_GT REL_GE WHILE WEND EXIT REPEAT UNTIL MIN MAX
%token BYTE HBYTE LBYTE LSHIFT RSHIFT MOD WAIT SUM SUMSQ POW AND OR BTRUE BFALSE
%token BAND BOR NOR NAND NOT XNOR XOR HEX BIN BIN8 OCT IFF EQV IMP ASC ON DEG RAD
%token I2C SPI START RESTART STOP INIT READ WRITE INT SGN COS SIN TAN ACOS ASIN ATAN
%token SINH COSH TANH ASINH ACOSH ATANH ATAN2 LOG LOG10 EXP SQRT FLOOR CEIL

%type <val> expression term factor sum_args sumsq_args
%type <ival> boolean_expr mode

// Operator precedence and associativity for arithmetic expressions.
%left AND OR
%left '=' REL_NE REL_LT REL_LE REL_GT REL_GE
%left '+' '-'
%left '*' '/' MOD BAND BOR NOR NAND XNOR XOR EQV IMP
%right UMINUS UPLUS INVERT NOT

%%

// Top-level grammar entry point.
program
    : line
    ;

// A line may be a numbered program line or an immediate command.
line
    : NUMBER statement CR
    | statement CR
    | CR
    ;

statement
    : PRINT '(' expr_list ')'          { if (!if_skip) str_print("\n"); }

    | PRINT '(' HEX ','                { if (!if_skip) current_print_mode = PRINT_HEX; }   
    expr_list ')'                      { if (!if_skip) { current_print_mode = PRINT_DEC; str_print("\n"); } }

    | PRINT '(' BIN ','                { if (!if_skip) current_print_mode = PRINT_BIN; }   
    expr_list ')'                      { if (!if_skip) { current_print_mode = PRINT_DEC; str_print("\n"); } }

    | PRINT '(' BIN8 ','               { if (!if_skip) current_print_mode = PRINT_BIN_GRP; }   
    expr_list ')'                      { if (!if_skip) { current_print_mode = PRINT_DEC; str_print("\n"); } }   

    | PRINT '(' OCT ','                { if (!if_skip) current_print_mode = PRINT_OCT; }   
    expr_list ')'                      { if (!if_skip) { current_print_mode = PRINT_DEC; str_print("\n"); } }  

    | ON expression ','                
        { 
            if ((running) && (!if_skip)) { 

                if($2.type != VAL_INT)
                    err_print("Required integer type\n");
                else {
                    on_goto_frame.target = $2.as.i; 
                    on_goto_frame.index = 0; 
                    on_goto_frame.target_num = -1;
                }
            } 
        }
    line_args      
        {
            if ((running) && (!if_skip)) {
                if(on_goto_frame.target_num >= 0) {
                    jump_pending = JUMP_GOSUB; 
                    jump_target = on_goto_frame.target_num;
                }
            }
        }

    | DELAY '(' expression ')'
        {
            if (!if_skip) {
                if($3.type != VAL_INT)
                    err_print("Required integer type\n");
                else {
                    int ms = $3.as.i;
                    if (ms < 0) ms = 0;
                    
                    platform_delay_ms(ms);
                }
            }
        }

    | PIN '(' expression ',' mode ')'
        {
            if (!if_skip) {
                if($3.type != VAL_INT)
                    err_print("Required integer type\n");
                else
                    platform_pin_mode($3.as.i, $5);
            }
        }

    | SET '(' expression ',' expression ')'
        {
            if (!if_skip) {
                if(($3.type != VAL_INT) || ($5.type != VAL_INT))
                    err_print("Required integer type\n");
                else
                    platform_digital_write($3.as.i, $5.as.i);
            }
        }

    | WAIT '(' expression ',' expression ')'
        {
            if (!if_skip) {
                while(is_continue) {
                    if(($3.type != VAL_INT) || ($5.type != VAL_INT))
                        err_print("Required integer type\n");
                    else {
                        if(platform_digital_read($3.as.i) == $5.as.i) {
                            break;
                        }                    
                        platform_delay_ms(10);
                    }
                }
            }
        }

    | I2C '(' INIT ')'     { if (!if_skip) platform_i2c_init();    }
    | I2C '(' START ')'    { if (!if_skip) platform_i2c_start();   }
    | I2C '(' RESTART ')'  { if (!if_skip) platform_i2c_restart(); }
    | I2C '(' STOP ')'     { if (!if_skip) platform_i2c_stop();    }
    | I2C '(' WRITE ',' expression ')' 
        { 
            if (!if_skip) {
                if($5.type != VAL_INT) 
                    err_print("Required integer type\n");
                else
                    if(!platform_i2c_write($5.as.i)) warn_print("Received NACK from I2C\n");
            }
        }

    | SPI '(' INIT ')'      { if (!if_skip) platform_spi_init();     }
    | SPI '(' START ')'     { if (!if_skip) platform_spi_select();   }
    | SPI '(' STOP ')'      { if (!if_skip) platform_spi_deselect(); }
    | SPI '(' WRITE ',' expression ')'
        {
            if (!if_skip) {
                if($5.type != VAL_INT) 
                    err_print("Required integer type\n");
                else 
                    platform_spi_read_buffer = platform_spi_transfer($5.as.i);
            }
        }

    | FOR VAR '=' expression TO expression
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    double start = to_float($4);
                    double limit = to_float($6);

                    var_set($2, make_float(start));

                    if (start > limit) {
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
                        loop_stack[loop_top].var = toupper($2);
                        loop_stack[loop_top].limit = limit;
                        loop_stack[loop_top].step = 1;
                        loop_stack[loop_top].ret_pc = pc + 1;

                        loop_top++;
                    }
                }
            }
        }

    | FOR VAR '=' expression TO expression STEP expression
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    double start = to_float($4);
                    double limit = to_float($6);
                    double step  = to_float($8);

                    if (step == 0) {
                        err_print("STEP cannot be zero\n");
                    }
                    else {
                        var_set($2, make_float(start));

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
                            loop_stack[loop_top].var = toupper($2);
                            loop_stack[loop_top].limit = limit;
                            loop_stack[loop_top].step = step;
                            loop_stack[loop_top].ret_pc = pc + 1;

                            loop_top++;
                        }
                    }
                }
            }
        }

    | NEXT VAR
       {
            if ((running) && (!if_skip)) {
                if (loop_top == 0) {
                    err_print("NEXT without matching FOR\n");
                }
                else if (loop_stack[loop_top - 1].type != LOOP_FOR) {
                    err_print("NEXT without matching FOR\n");
                }
                else if (loop_stack[loop_top - 1].var != toupper($2)) {
                    err_print("Mismatched NEXT variable\n");
                } else {
                    LoopFrame* f = &loop_stack[loop_top - 1];
                    double newval = to_float(var_get(f->var)) + f->step;
                    var_set(f->var, make_float(newval));

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

    | WHILE boolean_expr
        {
            if ((running) && (!if_skip)) {
                if (!$2) {

                    jump_target = find_end_node(program[pc].num, LINE_WHILE);

                    if (jump_target < 0)
                        err_print("Missing WEND\n");
                    else
                        jump_pending = JUMP_CONDITION_SKIP;
                }
            }
        }

    | WEND
        {
           if ((running) && (!if_skip)) {
                jump_target = find_root_node(program[pc].num, LINE_WHILE);

                if (jump_target < 0)
                    err_print("WEND without WHILE\n");
                else
                    jump_pending = JUMP_GOTO;
            }
        }

    | REPEAT 
        {
            if ((running) && (!if_skip)) {
                if(find_end_node(program[pc].num, LINE_REPEAT) < 0) {
                    err_print("Missing UNTIL\n");
                }
            }
        }

    | UNTIL boolean_expr
        {
            if ((running) && (!if_skip)) {
                if (!$2) {
                    jump_target = find_root_node(program[pc].num, LINE_REPEAT);

                    if (jump_target < 0)
                        err_print("Missing REPEAT\n");
                    else
                        jump_pending = JUMP_CONDITION_SKIP;
                }
            }
        }

    | EXIT 
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

    | IF boolean_expr THEN
        {
            // Only evaluate the condition if we're not already skipping due to an outer IF.
            if (!if_skip) {
                if_skip = !$2;
            }
        }
      statement

    | IF boolean_expr THEN
        {
            if ((running) && (!if_skip)) {
                if(!$2)
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

    | ELSE
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
    
    | ENDIF
        {
            if((running) && (!if_skip)) {
                if(find_root_node(program[pc].num, LINE_IF) == -1) {
                    if(program[pc].type != LINE_SINGLE_IF) {
                        err_print("ENDIF without IF\n");
                    }
                }
            }
        }

    | GOTO expression
        {
            if (!if_skip) { jump_pending = JUMP_GOTO; jump_target = to_int($2); }
        }

    | INPUT '(' var_list ')'

    | LET VAR '=' expression
        {
            if (!if_skip) var_set($2, $4);
        }

    | GOSUB expression
        {
            if (!if_skip) { jump_pending = JUMP_GOSUB; jump_target = to_int($2); }
        }

    | RETURN
        {
            if (!if_skip) { pc = stack_pop(); jump_pending = JUMP_RETURN; }
        }

    | NEW
        {
            if (!if_skip) { prog_size = 0; flush_memory(); }
        }

    | CLEAR
        {
            if (!if_skip) flush_memory(); 
        }

    | LIST
        {
            if (!if_skip) {
                for (int i = 0; i < prog_size; i++)
                    str_print("%d %s\n", program[i].num, program[i].text);
            }
        }

    | RUN   { if (!if_skip) { if(!running) { running = 1; pc = 0; } } }

    | END   { if (!if_skip) { if(!running) is_continue = 0; else running = 0; } }
    ;

expr_list
    : expr_item
    | expr_list ',' expr_item
    ;

expr_item
    : STRING
        {
            if (!if_skip) {
                char* s = $1;
                int len = strlen(s);

                // Strip surrounding quotes.
                if (len >= 2) {
                    s[len - 1] = '\0';
                    str_print("%s", s + 1);
                }
                free($1);
            }
        }
    | expression
        {
            if (!if_skip) {
                switch(current_print_mode)
                {
                    case PRINT_HEX:
                        str_print("%x", to_int($1));
                        break;
                    case PRINT_OCT:
                        str_print("%o", to_int($1));
                        break;
                    case PRINT_BIN:
                        {
                            char temp_buffer[INT_SIZE + 1];
                            int_to_binary(to_int($1), temp_buffer, 0);
                            str_print("%s", temp_buffer);
                        }
                        break;
                    case PRINT_BIN_GRP:
                        {
                            char temp_buffer[INT_SIZE + 13];
                            int_to_binary(to_int($1), temp_buffer, 1);
                            str_print("%s", temp_buffer);
                        }
                        break;
                    default:
                        str_print("%g", to_float($1));
                }
            }
        }
    ;

var_list
    : VAR
        {
            if (!if_skip) {
                int v;
                str_print("? ");
                
                v = int_input();
                var_set($1, make_int(v));
            }
        }
    | var_list ',' VAR
        {
           if (!if_skip) {
                int v;
                str_print("? ");
                
                v = int_input();
                var_set($3, make_int(v));
            }
        }
    ;

expression
    : term                       { $$ = $1;      }
    | '+' term   %prec UPLUS     { $$ = $2;      }
    | '-' term   %prec UMINUS    
        { 
            if ($2.type == VAL_FLOAT) {
                $$ = make_float(-$2.as.f);
            } else {
                $$ = make_int(-$2.as.i);
            }
        }
    | INVERT expression          { $$ = make_int(!(to_int($2)));   }
    | NOT expression             { $$ = make_int(~(to_int($2)));   }
    | expression '+' term        { DO_MATH_OP($$, $1, $3, +); }
    | expression '-' term        { DO_MATH_OP($$, $1, $3, -); }
    | expression '=' expression    { DO_REL_OP($$, $1, $3, ==); }
    | expression REL_NE expression { DO_REL_OP($$, $1, $3, !=); }
    | expression REL_LT expression { DO_REL_OP($$, $1, $3, <);  }
    | expression REL_LE expression { DO_REL_OP($$, $1, $3, <=); }
    | expression REL_GT expression { DO_REL_OP($$, $1, $3, >);  }
    | expression REL_GE expression { DO_REL_OP($$, $1, $3, >=); }
    | expression AND expression    { $$ = make_int((to_int($1) && to_int($3))); }
    | expression OR expression     { $$ = make_int((to_int($1) || to_int($3))); }
    ;

boolean_expr
    : expression                 { $$ = (to_float($1) != 0.0); }
    ;

mode
    : IN                         { $$ = PIN_MODE_INPUT;  }
    | OUT                        { $$ = PIN_MODE_OUTPUT; }
    ;

line_args
    : expression
        {
            if(on_goto_frame.index == on_goto_frame.target) on_goto_frame.target_num = to_int($1);
            on_goto_frame.index++;
        }
    | line_args ',' expression
        {
            if(on_goto_frame.index == on_goto_frame.target) on_goto_frame.target_num = to_int($3);
            on_goto_frame.index++;
        }

sum_args
    : expression
       {
            $$ = $1; 
       }
    | sum_args ',' expression
       {
            $$ = make_float(to_float($1) + to_float($3));
       }
    ;

sumsq_args
    : expression
       {
            $$ = make_float(to_float($1) * to_float($1)); 
       }
    | sumsq_args ',' expression
       {
            $$ = make_float(to_float($1) + (to_float($3) * to_float($3)));
       }
    ;

term
    : factor                     { $$ = $1; }
    | term '*' factor            { DO_MATH_OP($$, $1, $3, *); }
    | term '/' factor
        {
            if (to_float($3) == 0) { 
                err_print("Division by zero\n"); 
                $$ = make_float(0); 
            }
            else          
                DO_MATH_OP($$, $1, $3, /); 
        }

    | term MOD factor
        {
             if (to_float($3) == 0) { 
                err_print("Division by zero\n"); 
                $$ = make_float(0);  
            }
            else          
                $$ = make_float(to_int($1) % to_int($3));
        }
    
    | term BAND factor  { REQUIRE_INT_EX($$, $1, $3, ($1.as.i & $3.as.i));    }
    | term BOR factor   { REQUIRE_INT_EX($$, $1, $3, ($1.as.i | $3.as.i));    }
    | term NOR factor   { REQUIRE_INT_EX($$, $1, $3, !($1.as.i | $3.as.i));   }
    | term NAND factor  { REQUIRE_INT_EX($$, $1, $3, !($1.as.i & $3.as.i));   }
    | term XNOR factor  { REQUIRE_INT_EX($$, $1, $3, !($1.as.i ^ $3.as.i));   }
    | term XOR factor   { REQUIRE_INT_EX($$, $1, $3, ($1.as.i ^ $3.as.i));    }
    | term EQV factor   { REQUIRE_INT_EX($$, $1, $3, ~($1.as.i ^ $3.as.i));   }
    | term IMP factor   { REQUIRE_INT_EX($$, $1, $3, ((~$1.as.i) | $3.as.i)); }
    ;

factor
    : VAR                        { $$ = var_get($1);    }
    | NUMBER                     { $$ = make_int($1);   }
    | FLOAT_NUMBER               { $$ = make_float($1); }
    | '(' expression ')'         { $$ = $2; }
    | ANALOG '(' factor ')'      { REQUIRE_INT($$, $3, platform_analog_read($3.as.i));  }
    | GET '(' factor ')'         { REQUIRE_INT($$, $3, platform_digital_read($3.as.i)); }
    | RAND '(' ')'               { $$ = make_int(rand() % 32768); }
    | ABS '(' expression ')'     { $$ = make_float(abs(to_float($3))); }
    | MIN '(' expression ',' expression ')'         { $$ = make_float(min(to_float($3), to_float($5))); }
    | MAX '(' expression ',' expression ')'         { $$ = make_float(max(to_float($3), to_float($5))); }
    | BYTE '(' expression ')'    { REQUIRE_INT($$, $3, $3.as.i & 0xFF); }
    | HBYTE '(' expression ')'   { REQUIRE_INT($$, $3, $3.as.i & 0xF0); }
    | LBYTE '(' expression ')'   { REQUIRE_INT($$, $3, $3.as.i & 0x0F); }
    | LSHIFT '(' expression ',' expression  ')'     { REQUIRE_INT_EX($$, $3, $5, $3.as.i << $5.as.i); }
    | RSHIFT '(' expression ',' expression  ')'     { REQUIRE_INT_EX($$, $3, $5, $3.as.i >> $5.as.i); }
    | SUMSQ '(' sumsq_args ')'   { $$ = $3; }
    | SUM '(' sum_args ')'       { $$ = $3; }
    | POW '(' expression ',' expression ')'         { REQUIRE_INT_EX($$, $3, $5, power($3.as.i, $5.as.i)); }
    | IFF '(' boolean_expr ',' expression ',' expression ')'    { $$ = $3 ? $5 : $7; }
    | ASC '(' STRING ')'  
        {
            char* s = $3;
            $$ = make_int((strlen(s) > 2) ? s[1] : 0);
        }
    | I2C '(' READ ',' expression ')' { REQUIRE_INT($$, $5, platform_i2c_read($5.as.i)); }
    | SPI '(' READ ')'                { $$ = make_int(platform_spi_read_buffer);     }
    | INT '(' expression ')'          { $$ = make_int(to_int($3)); }
    | SGN '(' expression ')'
        {
            double tmp = to_float($3);
            if(tmp == 0) $$ = make_int(0);
            else if(tmp < 0) $$ = make_int(-1);
            else $$ = make_int(1);
        }
    | SIN '(' expression ')'     { $$ = make_float(platform_sin(to_float($3)));  }
    | COS '(' expression ')'     { $$ = make_float(platform_cos(to_float($3)));  }
    | TAN '(' expression ')'     { $$ = make_float(platform_tan(to_float($3)));  }
    | ASIN '(' expression ')'    { $$ = make_float(platform_asin(to_float($3))); }
    | ACOS '(' expression ')'    { $$ = make_float(platform_acos(to_float($3))); }
    | ATAN '(' expression ')'    { $$ = make_float(platform_atan(to_float($3))); }    

    | SINH '(' expression ')'    { $$ = make_float(platform_sinh(to_float($3)));  }
    | COSH '(' expression ')'    { $$ = make_float(platform_cosh(to_float($3)));  }
    | TANH '(' expression ')'    { $$ = make_float(platform_tanh(to_float($3)));  }
    | ASINH '(' expression ')'   { $$ = make_float(platform_asinh(to_float($3))); }
    | ACOSH '(' expression ')'   { $$ = make_float(platform_acosh(to_float($3))); }
    | ATANH '(' expression ')'   { $$ = make_float(platform_atanh(to_float($3))); }  

    | ATAN2 '(' expression ',' expression ')'   { $$ = make_float(platform_atan2(to_float($3), to_float($5))); }

    | LOG '(' expression ')'      { $$ = make_float(platform_log(to_float($3)));   }
    | LOG10 '(' expression ')'    { $$ = make_float(platform_log10(to_float($3))); }
    | EXP '(' expression ')'      { $$ = make_float(platform_exp(to_float($3)));   }
    | SQRT '(' expression ')'     { $$ = make_float(platform_sqrt(to_float($3)));  }
    | FLOOR '(' expression ')'    { $$ = make_float(platform_floor(to_float($3))); }
    | CEIL '(' expression ')'     { $$ = make_float(platform_ceil(to_float($3)));  }

    | DEG '(' expression ')'      { $$ = make_float(rad_to_deg(to_float($3))); }
    | RAD '(' expression ')'      { $$ = make_float(deg_to_rad(to_float($3))); }
    ;

%%

void yyerror(const char* s) { err_print("Error: %s\n", s); }

static void do_run(void) {
  jump_pending = JUMP_NONE;
  
  stack_top = 0;
  loop_top = 0;
  
  cjump_map_size = 0;
  pc = 0;
  is_continue = 1;

  current_print_mode = PRINT_DEC;

  platform_spi_read_buffer = 0xFF;

  build_conditional_jump_map();

#ifdef DEBUG
  printf("Conditional Jump Map\n");
  for(int i = 0; i < cjump_map_size; i++) {
      printf("Type=%d, Root=%d, End=%d, Else=%d\n", cjump_map[i].type, cjump_map[i].root_node_num, 
        cjump_map[i].end_node_num, cjump_map[i].else_node_num);
  }
  printf("--------------------------------\n");
#endif

  while (running && pc < prog_size) {
    char exec[MAX_LINE_LEN + 2];
    snprintf(exec, sizeof(exec), "%s\n", program[pc].text);

    jump_pending = JUMP_NONE;
    if_skip = 0;

    if(platform_is_key_pressed != NULL) {
        if(platform_is_key_pressed() == 0x1B) {
            running = 0;
            is_continue = 0;
            continue;
        }
    }

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