%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinybasic.tab.h"
%}

%option yylineno noyywrap

alpha [a-zA-Z]
digit [0-9]

%%

(?i:print)   { return PRINT;  }
(?i:if)      { return IF;     }
(?i:then)    { return THEN;   }
(?i:goto)    { return GOTO;   }
(?i:input)   { return INPUT;  }
(?i:let)     { return LET;    }
(?i:gosub)   { return GOSUB;  }
(?i:return)  { return RETURN; }
(?i:clear)   { return CLEAR;  }
(?i:list)    { return LIST;   }
(?i:run)     { return RUN;    }
(?i:end)     { return END;    }
(?i:rand)    { return RAND;   }
(?i:for)     { return FOR;    }
(?i:to)      { return TO;     }
(?i:step)    { return STEP;   }
(?i:next)    { return NEXT;   }

{alpha}      { yylval.cval = yytext[0]; return VAR; }           /* Single-letter variable. */
{digit}+     { yylval.ival = atoi(yytext); return NUMBER; }
\"[^\"]*\"   { yylval.sval = strdup(yytext); return STRING; }

"<="         { return REL_LE;       }
"<>"         { return REL_NE;    }
">="         { return REL_GE;       }

"<"          { return REL_LT;   }
">"          { return REL_GT;   }
"="          { return '=';      }

"+"          { return '+'; }
"-"          { return '-'; }
"*"          { return '*'; }
"/"          { return '/'; }
"("          { return '('; }
")"          { return ')'; }
","          { return ','; }

\n           { return CR; }
[ \t\r]+     { /* ignore. */ }

.            { fprintf(stderr, "Unknown character '%s' at line %d\n", yytext, yylineno); }
%%