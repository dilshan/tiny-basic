%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "platform.h"

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

extern YY_BUFFER_STATE yy_scan_string(const char *);
extern void yy_delete_buffer(YY_BUFFER_STATE);

extern int yylex(void);
extern int yylineno;

void yyerror(const char *s);

static int variables[26];
static inline int  var_get(char c) { return variables[toupper((unsigned char)c) - 'A']; }
static inline void var_set(char c, int v){ variables[toupper((unsigned char)c) - 'A'] = v; }

#define MAX_LINES    1000
#define MAX_LINE_LEN  256

typedef struct { int num; char text[MAX_LINE_LEN]; } Line;

static Line  program[MAX_LINES];
static int   prog_size = 0;
static int   pc        = 0;
static int   running   = 0;

static void prog_store(int num, const char *text) 
{
    int i;

    for (i = 0; i < prog_size; i++) 
    {
        if (program[i].num == num) 
        {
            strncpy(program[i].text, text, MAX_LINE_LEN-1);
            program[i].text[MAX_LINE_LEN-1] = '\0';
            return;
        }

        if (program[i].num > num) 
        {
            if (prog_size < MAX_LINES) 
            {
                memmove(&program[i+1], &program[i],(prog_size - i) * sizeof(Line));
                prog_size++;
                program[i].num = num;                
                strncpy(program[i].text, text, MAX_LINE_LEN-1);
                program[i].text[MAX_LINE_LEN-1] = '\0';
            }

            return;
        }
    }

    if (prog_size < MAX_LINES) 
    {
        program[prog_size].num = num;
        strncpy(program[prog_size].text, text, MAX_LINE_LEN-1);
        program[prog_size].text[MAX_LINE_LEN-1] = '\0';
        prog_size++;
    }
}

static int prog_find(int num) 
{
    for (int i = 0; i < prog_size; i++)
        if (program[i].num == num) return i;

    return -1;
}

static void prog_clear(void) 
{
    prog_size = 0; pc = 0;
    memset(variables, 0, sizeof(variables));
}

#define STACK_DEPTH 64
static int call_stack[STACK_DEPTH];
static int stack_top = 0;

static void stack_push(int ret_pc) 
{
    if (stack_top >= STACK_DEPTH) 
    { 
        PPRINT_ERR("GOSUB stack overflow\n"); 
        return; 
    }

    call_stack[stack_top++] = ret_pc;
}

static int stack_pop(void) 
{
    if (stack_top <= 0) 
    { 
        PPRINT_ERR("RETURN without GOSUB\n"); 
        return 0; 
    }

    return call_stack[--stack_top];
}

#define JUMP_NONE   0
#define JUMP_GOTO   1
#define JUMP_GOSUB  2
#define JUMP_RETURN 3

static int jump_pending = JUMP_NONE;
static int jump_target  = 0;
static int if_skip = 0;
static int is_continue = 1;

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
%token RAND
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
    : PRINT expr_list          { if (!if_skip) PPRINTF("\n"); }

    | IF expression relop expression
        {
            // Only evaluate the condition if we're not already skipping due to an outer IF.
            if (!if_skip)
            {
                int expression_result;
                switch ($3) {
                    case 0: expression_result = ($2 <  $4); break;
                    case 1: expression_result = ($2 <= $4); break;
                    case 2: expression_result = ($2 != $4); break;
                    case 3: expression_result = ($2 >  $4); break;
                    case 4: expression_result = ($2 >= $4); break;
                    case 5: expression_result = ($2 == $4); break;
                    default: expression_result = 0;
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
                    PPRINTF("%d %s\n", program[i].num, program[i].text);
            }
        }

    | RUN   { if (!if_skip) { running = 1; pc = 0; } }

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
                char *s = $1;
                int len = strlen(s);
                
                // Strip surrounding quotes.
                if (len >= 2) { s[len-1] = '\0'; PPRINTF("%s", s+1); }
                free($1);
            }
        }
    | expression
        {
            if (!if_skip) PPRINTF("%d", $1);
        }
    ;

var_list
    : VAR
        {
            if (!if_skip) 
            {
                int v; PPRINTF("? "); fflush(stdout);
                if (PSCANF("%d", &v) == 1) 
                    var_set($1, v);
                
                // Consume the rest of the line to avoid affecting the next input.
                PSCANF("%*c");
            }
        }
    | var_list ',' VAR
        {
            if (!if_skip) 
            {
                int v; PPRINTF("? "); 
                fflush(stdout);

                if (PSCANF("%d", &v) == 1) 
                    var_set($3, v);
                
                // See above: consume the rest of the line.
                PSCANF("%*c");
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
    | RAND                       { $$ = RANDOM() % 32768; }
    ;

term
    : factor                     { $$ = $1; }
    | term '*' factor            { $$ = $1 * $3; }
    | term '/' factor
        {
            if ($3 == 0) { PPRINT_ERR("Division by zero\n"); $$ = 0; }
            else          $$ = $1 / $3;
        }
    ;

factor
    : VAR                        { $$ = var_get($1); }
    | NUMBER                     { $$ = $1; }
    | '(' expression ')'         { $$ = $2; }
    ;

%%

void yyerror(const char *s) 
{
    PPRINT_ERR("Error: %s\n", s);
}

static void do_run(void) 
{
    jump_pending = JUMP_NONE;
    stack_top    = 0;
    pc           = 0;

    while (running && pc < prog_size) 
    {
        char exec[MAX_LINE_LEN + 2];
        snprintf(exec, sizeof(exec), "%s\n", program[pc].text);

        jump_pending = JUMP_NONE;
        if_skip      = 0;

        YY_BUFFER_STATE buf = yy_scan_string(exec);
        yyparse();
        yy_delete_buffer(buf);

        if (!running) break;

        switch (jump_pending) 
        {
            case JUMP_GOTO: 
            {
                int idx = prog_find(jump_target);
                if (idx < 0) 
                { 
                    PPRINT_ERR("Undefined line %d\n", jump_target); 
                    goto done; 
                }

                pc = idx;
                break;
            }

            case JUMP_GOSUB: 
            {
                int idx = prog_find(jump_target);
                if (idx < 0) 
                { 
                    PPRINT_ERR("Undefined line %d\n", jump_target); 
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

int main(void) 
{
    char line[MAX_LINE_LEN];
    memset(variables, 0, sizeof(variables));
    PPRINTF("Tiny BASIC  (type END or Ctrl-D to quit)\n");

    while (is_continue) 
    {
        PPRINTF("> "); fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) 
            break;

        // strip trailing newline.
        int len = strlen(line);
        if (len > 0 && line[len-1] == '\n') 
        { 
            line[--len] = '\0'; 
        }

        // skip blank input.
        char *p = line;
        while (*p == ' ' || *p == '\t') 
            p++;
        
        if (*p == '\0') 
            continue;

        if (isdigit((unsigned char)*p)) 
        {
            // Numbered line: store it, don't execute.
            int num = 0;
            while (isdigit((unsigned char)*p)) 
                num = num*10 + (*p++ - '0');

            while (*p == ' ' || *p == '\t') 
                p++;

            prog_store(num, p);
        } 
        else 
        {
            // Immediate mode: parse and execute.
            char with_nl[MAX_LINE_LEN + 2];

            snprintf(with_nl, sizeof(with_nl), "%s\n", line);

            running = 0; 
            jump_pending = JUMP_NONE; 
            if_skip = 0;

            YY_BUFFER_STATE buf = yy_scan_string(with_nl);
            yyparse();
            yy_delete_buffer(buf);

            if (running) 
                do_run();
        }
    }

    PPRINTF("\n");
    return 0;
}