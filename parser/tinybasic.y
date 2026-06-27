%{
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

%}

%union {
    int   ival;     /* For numeric literals. */
    char  cval;     /* For single-letter variables. */
    char* sval;     /* For quoted strings. */
}

// Terminals for BASIC keywords and operators.
%token <ival> NUMBER
%token <cval> VAR
%token <sval> STRING

%token PRINT IF THEN ELSE ENDIF GOTO INPUT LET GOSUB RETURN CLEAR LIST RUN END CR
%token NEW RAND FOR TO STEP NEXT DELAY ANALOG HIGH LOW PIN IN OUT GET SET ABS
%token REL_LT REL_LE REL_NE REL_GT REL_GE WHILE WEND EXIT REPEAT UNTIL MIN MAX
%token BYTE HBYTE LBYTE LSHIFT RSHIFT MOD WAIT SUM SUMSQ POW AND OR

%type <ival> expression term factor boolean_expr mode sum_args sumsq_args

// Operator precedence and associativity for arithmetic expressions.
%left AND OR
%left '=' REL_NE REL_LT REL_LE REL_GT REL_GE
%left '+' '-'
%left '*' '/' MOD
%right UMINUS UPLUS INVERT

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

    | DELAY '(' expression ')'
        {
            if (!if_skip) {
                int ms = $3;
                if (ms < 0) ms = 0;
                
                platform_delay_ms(ms);
            }
        }

    | PIN '(' expression ',' mode ')'
        {
            if (!if_skip) {
                platform_pin_mode($3, $5);
            }
        }

    | SET '(' expression ',' expression ')'
        {
            if (!if_skip) {
                platform_digital_write($3, $5);
            }
        }

    | WAIT '(' expression ',' expression ')'
        {
            if (!if_skip) {
                while(is_continue) {
                    if(platform_digital_read($3) == $5) {
                        break;
                    }                    
                    platform_delay_ms(10);
                }
            }
        }

    | FOR VAR '=' expression TO expression
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    int start = $4;
                    int limit = $6;
                    int step  = 1;

                    var_set($2, start);

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

    | FOR VAR '=' expression TO expression STEP expression
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    int start = $4;
                    int limit = $6;
                    int step  = $8;

                    if (step == 0) {
                        err_print("STEP cannot be zero\n");
                    }
                    else {
                        var_set($2, start);

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
            if (!if_skip) { jump_pending = JUMP_GOTO; jump_target = $2; }
        }

    | INPUT '(' var_list ')'

    | LET VAR '=' expression
        {
            if (!if_skip) var_set($2, $4);
        }

    | GOSUB expression
        {
            if (!if_skip) { jump_pending = JUMP_GOSUB; jump_target = $2; }
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
            if (!if_skip) str_print("%d", $1);
        }
    ;

var_list
    : VAR
        {
            if (!if_skip) {
                int v;
                str_print("? ");
                
                v = int_input();
                var_set($1, v);
            }
        }
    | var_list ',' VAR
        {
           if (!if_skip) {
                int v;
                str_print("? ");
                
                v = int_input();
                var_set($3, v);
            }
        }
    ;

expression
    : term                       { $$ = $1;      }
    | '+' term   %prec UPLUS     { $$ = $2;      }
    | '-' term   %prec UMINUS    { $$ = -$2;     }
    | INVERT expression          { $$ = !($2);   }
    | expression '+' term        { $$ = $1 + $3; }
    | expression '-' term        { $$ = $1 - $3; }
    | expression '=' expression    { $$ = ($1 == $3); }
    | expression REL_NE expression { $$ = ($1 != $3); }
    | expression REL_LT expression { $$ = ($1 < $3); }
    | expression REL_LE expression { $$ = ($1 <= $3); }
    | expression REL_GT expression { $$ = ($1 > $3); }
    | expression REL_GE expression { $$ = ($1 >= $3); }
    | expression AND expression    { $$ = ($1 && $3); }
    | expression OR expression     { $$ = ($1 || $3); }
    ;

boolean_expr
    : expression                 { $$ = ($1 != 0); }
    ;

mode
    : IN                         { $$ = PIN_MODE_INPUT;  }
    | OUT                        { $$ = PIN_MODE_OUTPUT; }
    ;

sum_args
    : expression
       {
            $$ = $1; 
       }
    | sum_args ',' expression
       {
            $$ = $1 + $3;
       }
    ;

sumsq_args
    : expression
       {
            $$ = ($1 * $1); 
       }
    | sumsq_args ',' expression
       {
            $$ = $1 + ($3 * $3);
       }
    ;

term
    : factor                     { $$ = $1; }
    | term '*' factor            { $$ = $1 * $3; }
    | term '/' factor
        {
            if ($3 == 0) { 
                err_print("Division by zero\n"); 
                $$ = 0; 
            }
            else          
                $$ = $1 / $3;
        }
    | term MOD factor
        {
             if ($3 == 0) { 
                err_print("Division by zero\n"); 
                $$ = 0; 
            }
            else          
                $$ = $1 % $3;
        }
    ;

factor
    : VAR                        { $$ = var_get($1); }
    | NUMBER                     { $$ = $1; }
    | '(' expression ')'         { $$ = $2; }
    | ANALOG '(' factor ')'      { $$ = platform_analog_read($3); }
    | GET '(' factor ')'         { $$ = platform_digital_read($3); }
    | HIGH                       { $$ = 1; }
    | LOW                        { $$ = 0; }
    | RAND '(' ')'               { $$ = rand() % 32768; }
    | ABS '(' expression ')'     { $$ = abs($3); }
    | MIN '(' expression ',' expression ')'         { $$ = min($3, $5); }
    | MAX '(' expression ',' expression ')'         { $$ = max($3, $5); }
    | BYTE '(' expression ')'    { $$ = $3 & 0xFF; }
    | HBYTE '(' expression ')'   { $$ = $3 & 0xF0; }
    | LBYTE '(' expression ')'   { $$ = $3 & 0x0F; }
    | LSHIFT '(' expression ',' expression  ')'     { $$ = $3 << $5; }
    | RSHIFT '(' expression ',' expression  ')'     { $$ = $3 >> $5; }
    | SUMSQ '(' sumsq_args ')'   { $$ = $3; }
    | SUM '(' sum_args ')'       { $$ = $3; }
    | POW '(' expression ',' expression ')'         { $$ = power($3, $5); }
    ;

%%

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