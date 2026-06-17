/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_TINYBASIC_TAB_H_INCLUDED
# define YY_YY_TINYBASIC_TAB_H_INCLUDED
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
    GOTO = 264,                    /* GOTO  */
    INPUT = 265,                   /* INPUT  */
    LET = 266,                     /* LET  */
    GOSUB = 267,                   /* GOSUB  */
    RETURN = 268,                  /* RETURN  */
    CLEAR = 269,                   /* CLEAR  */
    LIST = 270,                    /* LIST  */
    RUN = 271,                     /* RUN  */
    END = 272,                     /* END  */
    CR = 273,                      /* CR  */
    RAND = 274,                    /* RAND  */
    FOR = 275,                     /* FOR  */
    TO = 276,                      /* TO  */
    STEP = 277,                    /* STEP  */
    NEXT = 278,                    /* NEXT  */
    REL_LT = 279,                  /* REL_LT  */
    REL_LE = 280,                  /* REL_LE  */
    REL_NE = 281,                  /* REL_NE  */
    REL_GT = 282,                  /* REL_GT  */
    REL_GE = 283,                  /* REL_GE  */
    UMINUS = 284,                  /* UMINUS  */
    UPLUS = 285                    /* UPLUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 139 "tinybasic.y"

    int   ival;     /* For numeric literals. */
    char  cval;     /* For single-letter variables. */
    char* sval;     

#line 100 "tinybasic.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_TINYBASIC_TAB_H_INCLUDED  */
