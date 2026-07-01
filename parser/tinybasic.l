%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"
#include "tinybasic.tab.h"
#include "platform.h"
%}

%option yylineno noyywrap noinput nounput

alpha [a-zA-Z]

decimal [0-9]
octal   [0-7]
binary  [01]
hex     [0-9a-fA-F]

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
(?i:band)    { return BAND;   }
(?i:bor)     { return BOR;    }
(?i:nor)     { return NOR;    }
(?i:nand)    { return NAND;   }
(?i:not)     { return NOT;    }
(?i:xnor)    { return XNOR;   }
(?i:xor)     { return XOR;    }
(?i:hex)     { return HEX;    }
(?i:bin)     { return BIN;    }
(?i:bin8)    { return BIN8;   }
(?i:oct)     { return OCT;    }
(?i:iff)     { return IFF;    }
(?i:eqv)     { return EQV;    }
(?i:imp)     { return IMP;    }
(?i:asc)     { return ASC;    }
(?i:on)      { return ON;     }
(?i:i2c)     { return I2C;    }
(?i:write)   { return WRITE;  }
(?i:read)    { return READ;   }
(?i:stop)    { return STOP;   }
(?i:start)   { return START;  }
(?i:restart) { return RESTART; }
(?i:init)    { return INIT;   }
(?i:spi)     { return SPI;    }
(?i:int)     { return INT;    }
(?i:sgn)     { return SGN;    }
(?i:cos)     { return COS;    }
(?i:sin)     { return SIN;    }
(?i:tan)     { return TAN;    }
(?i:acos)    { return ACOS;   }
(?i:asin)    { return ASIN;   }
(?i:atan)    { return ATAN;   }
(?i:cosh)    { return COSH;   }
(?i:sinh)    { return SINH;   }
(?i:tanh)    { return TANH;   } 
(?i:acosh)   { return ACOSH;  }
(?i:asinh)   { return ASINH;  }
(?i:atanh)   { return ATANH;  } 
(?i:atan2)   { return ATAN2;  }
(?i:log)     { return LOG;    }
(?i:log10)   { return LOG10;  }
(?i:exp)     { return EXP;    }
(?i:sqrt)    { return SQRT;   }
(?i:floor)   { return FLOOR;  }
(?i:ceil)    { return CEIL;   }
(?i:deg)     { return DEG;    }
(?i:rad)     { return RAD;    }

(?i:pi)      { yylval.fval = 3.14159265358979323846; return FLOAT_NUMBER; }
(?i:e)       { yylval.fval = 2.71828182845904523536; return FLOAT_NUMBER; }
(?i:high)    { yylval.ival = 1; return NUMBER; }
(?i:low)     { yylval.ival = 0; return NUMBER; }
(?i:true)    { yylval.ival = 1; return NUMBER; }
(?i:false)   { yylval.ival = 0; return NUMBER; }

{alpha}      { yylval.cval = yytext[0]; return VAR; }           /* Single-letter variable. */

0[xX]{hex}+ {
    yylval.ival = (int)strtol(yytext, NULL, 16);
    return NUMBER;
}

0[bB]{binary}+ {
    yylval.ival = (int)strtol(yytext + 2, NULL, 2); 
    return NUMBER;
}

0[oO]{octal}+ {
    yylval.ival = (int)strtol(yytext + 2, NULL, 8);
    return NUMBER;
}

([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*) {
    yylval.fval = atof(yytext);
    return FLOAT_NUMBER;
}

{decimal}+ {
    yylval.ival = (int)strtol(yytext, NULL, 10);
    return NUMBER;
}

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