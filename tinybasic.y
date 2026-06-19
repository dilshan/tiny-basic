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

#define MAX_LINES 1000
#define MAX_LINE_LEN 256

#define STACK_DEPTH 32
#define FOR_STACK_DEPTH 16

#define JUMP_NONE 0
#define JUMP_GOTO 1
#define JUMP_GOSUB 2
#define JUMP_RETURN 3

typedef struct {
  short num;
  char text[MAX_LINE_LEN];
} Line;

typedef struct {
  char var;
  int limit;
  int step;
  unsigned short ret_pc;
} ForFrame;

extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

extern int yylex(void);
extern int yylineno;

static int variables[26];

static Line program[MAX_LINES];
static ForFrame for_stack[FOR_STACK_DEPTH];

static unsigned char jump_pending = JUMP_NONE;
static short jump_target = 0;
static unsigned char if_skip = 0;
static unsigned char is_continue = 1;

static unsigned short prog_size = 0;
static short pc = 0;
static unsigned char running = 0;
static char for_top = 0;

static short call_stack[STACK_DEPTH];
static short stack_top = 0;

void yyerror(const char* s);

static inline int var_get(char c) {
  return variables[toupper((unsigned char)c) - 'A'];
}
static inline void var_set(char c, int v) {
  variables[toupper((unsigned char)c) - 'A'] = v;
}

static void prog_store(int num, const char* text) {
  int i;

  for (i = 0; i < prog_size; i++) {
    if (program[i].num == num) {
      strncpy(program[i].text, text, MAX_LINE_LEN - 1);
      program[i].text[MAX_LINE_LEN - 1] = '\0';
      return;
    }

    if (program[i].num > num) {
      if (prog_size < MAX_LINES) {
        memmove(&program[i + 1], &program[i], (prog_size - i) * sizeof(Line));
        prog_size++;
        program[i].num = num;
        strncpy(program[i].text, text, MAX_LINE_LEN - 1);
        program[i].text[MAX_LINE_LEN - 1] = '\0';
      }

      return;
    }
  }

  if (prog_size < MAX_LINES) {
    program[prog_size].num = num;
    strncpy(program[prog_size].text, text, MAX_LINE_LEN - 1);
    program[prog_size].text[MAX_LINE_LEN - 1] = '\0';
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
  memset(variables, 0, sizeof(variables));
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
    char* sval;     
}

%token <ival> NUMBER
%token <cval> VAR
%token <sval> STRING

%token PRINT IF THEN GOTO INPUT LET GOSUB RETURN CLEAR LIST RUN END CR
%token RAND FOR TO STEP NEXT
%token REL_LT REL_LE REL_NE REL_GT REL_GE

%type <ival> expression term factor relop

%left '+' '-'
%left '*' '/'
%right UMINUS UPLUS

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

    | FOR VAR '=' expression TO expression
        {
            if ((running) && (!if_skip)) {
                if (for_top >= FOR_STACK_DEPTH) {
                    err_print("FOR stack overflow\n");
                } else {
                    var_set($2, $4);
                    for_stack[for_top].var = toupper($2);
                    for_stack[for_top].limit = $6;
                    for_stack[for_top].step = 1;
                    for_stack[for_top].ret_pc = pc + 1;
                    for_top++;
                }
            }
        }
    | FOR VAR '=' expression TO expression STEP expression
        {
            if ((running) && (!if_skip)) {
                if (for_top >= FOR_STACK_DEPTH) {
                    err_print("FOR stack overflow\n");
                } else {
                    var_set($2, $4);
                    for_stack[for_top].var = toupper($2);
                    for_stack[for_top].limit = $6;
                    for_stack[for_top].step = $8;
                    for_stack[for_top].ret_pc = pc + 1;
                    for_top++;
                }
            }
        }
    | NEXT VAR
       {
            if ((running) && (!if_skip)) {
                if (for_top <= 0 || for_stack[for_top - 1].var != toupper($2)) {
                    err_print("NEXT without matching FOR\n");
                } else {
                    ForFrame* f = &for_stack[for_top - 1];
                    int newval = var_get(f->var) + f->step;
                    var_set(f->var, newval);

                    int done = (f->step > 0) ? (newval > f->limit) : (newval < f->limit);
                    if (!done) {
                    jump_pending = JUMP_GOTO;
                    jump_target = program[f->ret_pc].num;
                    } else {
                    for_top--;
                    }
                }
            }
       }

    | IF expression relop expression
        {
            // Only evaluate the condition if we're not already skipping due to an outer IF.
            if (!if_skip) {
                int expression_result;
                switch ($3) {
                    case 0:
                    expression_result = ($2 < $4);
                    break;
                    case 1:
                    expression_result = ($2 <= $4);
                    break;
                    case 2:
                    expression_result = ($2 != $4);
                    break;
                    case 3:
                    expression_result = ($2 > $4);
                    break;
                    case 4:
                    expression_result = ($2 >= $4);
                    break;
                    case 5:
                    expression_result = ($2 == $4);
                    break;
                    default:
                    expression_result = 0;
                }

                if_skip = !expression_result;
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
                
                if (int_input()) var_set($1, v);
            }
        }
    | var_list ',' VAR
        {
           if (!if_skip) {
                int v;
                str_print("? ");
                
                if (int_input()) var_set($3, v);
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
    | RAND                       { $$ = rand() % 32768; }
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
    ;

%%

void yyerror(const char* s) { err_print("Error: %s\n", s); }

static void do_run(void) {
  jump_pending = JUMP_NONE;
  stack_top = 0;
  pc = 0;

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

int main(void) {

  str_print = printf;
  err_print = printf;  

  char line[MAX_LINE_LEN];
  memset(variables, 0, sizeof(variables));
  str_print("Tiny BASIC  (type END or Ctrl-D to quit)\n");

  while (is_continue) {
    str_print("> ");
    fflush(stdout);

    if (!fgets(line, sizeof(line), stdin)) break;

    // strip trailing newline.
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[--len] = '\0';
    }

    // skip blank input.
    char* p = line;
    while (*p == ' ' || *p == '\t') p++;

    if (*p == '\0') continue;

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

  str_print("\n");
  return 0;
}