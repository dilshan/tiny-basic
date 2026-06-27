%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinybasic.tab.h"

#include "platform.h"
%}

%option yylineno noyywrap noinput nounput

alpha [a-zA-Z]
digit [0-9]

%%

(?i:print)   { return PRINT;  }
(?i:if)      { return IF;     }
(?i:then)    { return THEN;   }
(?i:else)    { return ELSE;   }
(?i:endif)   { return ENDIF;  }
(?i:goto)    { return GOTO;   }
(?i:input)   { return INPUT;  }
(?i:let)     { return LET;    }
(?i:gosub)   { return GOSUB;  }
(?i:return)  { return RETURN; }
(?i:clear)   { return CLEAR;  }
(?i:new)     { return NEW;    }
(?i:list)    { return LIST;   }
(?i:run)     { return RUN;    }
(?i:end)     { return END;    }
(?i:rand)    { return RAND;   }
(?i:for)     { return FOR;    }
(?i:to)      { return TO;     }
(?i:step)    { return STEP;   }
(?i:next)    { return NEXT;   }
(?i:delay)   { return DELAY;  }
(?i:analog)  { return ANALOG; }
(?i:pin)     { return PIN;    }
(?i:get)     { return GET;    }
(?i:set)     { return SET;    }
(?i:high)    { return HIGH;   }
(?i:low)     { return LOW;    }
(?i:in)      { return IN;     }
(?i:out)     { return OUT;    }
(?i:while)   { return WHILE;  }
(?i:wend)    { return WEND;   }
(?i:abs)     { return ABS;    }
(?i:exit)    { return EXIT;   }
(?i:repeat)  { return REPEAT; }
(?i:until)   { return UNTIL;  }
(?i:min)     { return MIN;    }
(?i:max)     { return MAX;    }
(?i:byte)    { return BYTE;   }
(?i:hbyte)   { return HBYTE;  }
(?i:lbyte)   { return LBYTE;  }
(?i:lshift)  { return LSHIFT; }
(?i:rshift)  { return RSHIFT; }
(?i:mod)     { return MOD;    }
(?i:wait)    { return WAIT;   }
(?i:sum)     { return SUM;    }
(?i:sumsq)   { return SUMSQ;  }
(?i:pow)     { return POW;    }
(?i:and)     { return AND;    }
(?i:or)      { return OR;     }
(?i:true)    { return BTRUE;  }
(?i:false)   { return BFALSE; }

{alpha}      { yylval.cval = yytext[0]; return VAR; }           /* Single-letter variable. */
{digit}+     { yylval.ival = atoi(yytext); return NUMBER; }
\"[^\"]*\"   { yylval.sval = strdup(yytext); return STRING; }

"<="         { return REL_LE; }
"<>"         { return REL_NE; }
">="         { return REL_GE; }

"<"          { return REL_LT; }
">"          { return REL_GT; }
"="          { return '=';    }

"+"          { return '+'; }
"-"          { return '-'; }
"*"          { return '*'; }
"/"          { return '/'; }
"("          { return '('; }
")"          { return ')'; }
","          { return ','; }

"!"          { return INVERT; }

\n           { return CR; }
[ \t\r]+     { /* ignore. */ }

.            { err_print("Unknown character '%s' at line %d\n", yytext, yylineno); }
%%