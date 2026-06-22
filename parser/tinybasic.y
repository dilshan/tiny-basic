%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state* YY_BUFFER_STATE;
#endif

#define STACK_DEPTH 32
#define LOOP_STACK_DEPTH 16

#define JUMP_NONE 0
#define JUMP_GOTO 1
#define JUMP_GOSUB 2
#define JUMP_RETURN 3
#define JUMP_CONDITION 4

typedef enum {
    LOOP_FOR,
    LOOP_WHILE
} LoopType;

typedef enum {
    LINE_OTHER = 0,
    LINE_FOR,
    LINE_NEXT,
    LINE_WHILE,
    LINE_WEND
} LineType;

typedef struct {
    LineType type;
    short root_node_num;
    short end_node_num;
} ConditionalJumpMap;

typedef struct {
  short num;
  LineType type;
  char text[MAX_LINE_LEN];
} Line;

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

static int variables[26];

static Line program[MAX_LINES];
static LoopFrame loop_stack[LOOP_STACK_DEPTH];
static ConditionalJumpMap cjump_map[128];

static unsigned char jump_pending = JUMP_NONE;
static short jump_target = 0;
static unsigned char if_skip = 0;
static unsigned char is_continue = 1;

static unsigned short prog_size = 0;
static short pc = 0;
static unsigned char running = 0;
static size_t loop_top = 0;
static unsigned char cjump_map_size;

static short call_stack[STACK_DEPTH];
static short stack_top = 0;

void yyerror(const char* s);

static inline int var_get(char c) {
  return variables[toupper((unsigned char)c) - 'A'];
}
static inline void var_set(char c, int v) {
  variables[toupper((unsigned char)c) - 'A'] = v;
}

static LineType get_prog_line_type(const char* line) {
    while (isspace((unsigned char)*line))
        line++;

    if (strncasecmp(line, "FOR", 3) == 0)
        return LINE_FOR;
    else if (strncasecmp(line, "NEXT", 4) == 0)
        return LINE_NEXT;
    else if (strncasecmp(line, "WHILE", 5) == 0)
        return LINE_WHILE;
    else if (strncasecmp(line, "WEND", 4) == 0) 
        return LINE_WEND;
    else
        return LINE_OTHER;
}

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

static void prog_clear(void) {
  prog_size = 0;
  pc = 0;
  cjump_map_size = 0;
  memset(variables, 0, sizeof(variables));
}

static void build_conditional_jump_map(void) {
    LineType expected_root_node = LINE_OTHER;
    cjump_map_size = 0;

    for (int i = 0; i < prog_size; i++) {
        if(program[i].type != LINE_OTHER) {

            if((program[i].type == LINE_FOR) || (program[i].type == LINE_WHILE)) {
                cjump_map[cjump_map_size].type = program[i].type;
                cjump_map[cjump_map_size].root_node_num = program[i].num;
                cjump_map[cjump_map_size].end_node_num = -1;
                cjump_map_size++;
            }
            else
            {
                if(program[i].type == LINE_NEXT) expected_root_node = LINE_FOR;
                else if(program[i].type == LINE_WEND) expected_root_node = LINE_WHILE; 

                for(int j = (cjump_map_size - 1); j >= 0; j--) {
                    if((cjump_map[j].type == expected_root_node) && (cjump_map[j].end_node_num == -1)) {
                        cjump_map[j].end_node_num = program[i].num;
                        break;
                    }
                }
            }
        }
    }
}

static short find_end_node(short parent_node_num, LineType node_type) {
    for(int i = 0; i < cjump_map_size; i++) {
        if((cjump_map[i].type == node_type) && (cjump_map[i].root_node_num == parent_node_num)) {
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

static int eval_condition(int lhs, int op, int rhs) {
    switch (op) {
        case 0: return lhs < rhs;
        case 1: return lhs <= rhs;
        case 2: return lhs != rhs;
        case 3: return lhs > rhs;
        case 4: return lhs >= rhs;
        case 5: return lhs == rhs;
    }

    return 0;
}


%}

%union {
    int   ival;     /* For numeric literals. */
    char  cval;     /* For single-letter variables. */
    char* sval;     
}

%token <ival> NUMBER
%token <cval> VAR
%token <sval> STRING

%token PRINT IF THEN GOTO INPUT LET GOSUB RETURN CLEAR LIST RUN END CR
%token RAND FOR TO STEP NEXT DELAY ANALOG HIGH LOW PIN IN OUT GET SET
%token REL_LT REL_LE REL_NE REL_GT REL_GE WHILE WEND

%type <ival> expression term factor relop mode

%left '+' '-'
%left '*' '/'
%right UMINUS UPLUS INVERT

%%

program
    : line
    ;

line
    : NUMBER statement CR
    | statement CR
    | CR
    ;

statement
    : PRINT expr_list          { if (!if_skip) str_print("\n"); }

    | DELAY expression
        {
            if (!if_skip) {
                int ms = $2;
                if (ms < 0) ms = 0;
                
                platform_delay_ms(ms);
            }
        }

    | PIN expression ',' mode
        {
            if (!if_skip) {
                platform_pin_mode($2, $4);
            }
        }

    | SET expression ',' expression
        {
            if (!if_skip) {
                platform_digital_write($2, $4);
            }
        }

    | FOR VAR '=' expression TO expression
        {
            if ((running) && (!if_skip)) {
                if (loop_top >= LOOP_STACK_DEPTH) {
                    err_print("LOOP stack overflow\n");
                } else {
                    if ($4 > $6) {
                        jump_target = find_end_node(program[pc].num, program[pc].type);
                        jump_pending = JUMP_CONDITION;
                    }
                    else {                    
                        var_set($2, $4);
                        loop_stack[loop_top].type = LOOP_FOR;
                        loop_stack[loop_top].var = toupper($2);
                        loop_stack[loop_top].limit = $6;
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
                    if(($8 > 0) ? ($4 > $6) : ($4 < $6)) {
                        jump_target = find_end_node(program[pc].num, program[pc].type);
                        jump_pending = JUMP_CONDITION;
                    }
                    else {
                        var_set($2, $4);
                        loop_stack[loop_top].type = LOOP_FOR;
                        loop_stack[loop_top].var = toupper($2);
                        loop_stack[loop_top].limit = $6;
                        loop_stack[loop_top].step = $8;
                        loop_stack[loop_top].ret_pc = pc + 1;
                        loop_top++;
                    }
                }
            }
        }

    | NEXT VAR
       {
            if ((running) && (!if_skip)) {
                if (loop_top <= 0 || loop_stack[loop_top - 1].type != LOOP_FOR || loop_stack[loop_top - 1].var != toupper($2)) {
                    err_print("NEXT without matching FOR\n");
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

    | WHILE expression relop expression
        {
            if (!if_skip) {
                
            }
        }

    | IF expression relop expression
        {
            // Only evaluate the condition if we're not already skipping due to an outer IF.
            if (!if_skip) {
                if_skip = !eval_condition($2, $3, $4);
            }
        }
      THEN statement

    | GOTO expression
        {
            if (!if_skip) { jump_pending = JUMP_GOTO; jump_target = $2; }
        }

    | INPUT var_list

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

    | CLEAR
        {
            if (!if_skip) prog_clear();
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

relop
    : REL_LT    { $$ = 0; }
    | REL_LE    { $$ = 1; }
    | REL_NE    { $$ = 2; }
    | REL_GT    { $$ = 3; }
    | REL_GE    { $$ = 4; }
    | '='       { $$ = 5; }
    ;

expression
    : term                       { $$ = $1; }
    | '+' term   %prec UPLUS     { $$ = $2; }
    | '-' term   %prec UMINUS    { $$ = -$2; }
    | expression '+' term        { $$ = $1 + $3; }
    | expression '-' term        { $$ = $1 - $3; }
    ;

mode
    : IN                         { $$ = PIN_MODE_INPUT;  }
    | OUT                        { $$ = PIN_MODE_OUTPUT; }
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
    ;

factor
    : VAR                        { $$ = var_get($1); }
    | NUMBER                     { $$ = $1; }
    | '(' expression ')'         { $$ = $2; }
    | ANALOG factor              { $$ = platform_analog_read($2); }
    | GET factor                 { $$ = platform_digital_read($2); }
    | INVERT factor              { $$ = !($2); }
    | HIGH                       { $$ = 1; }
    | LOW                        { $$ = 0; }
    | RAND                       { $$ = rand() % 32768; }
    ;

%%

void yyerror(const char* s) { err_print("Error: %s\n", s); }

static void do_run(void) {
  jump_pending = JUMP_NONE;
  stack_top = 0;
  cjump_map_size = 0;
  pc = 0;

  build_conditional_jump_map();

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

      case JUMP_CONDITION: {
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