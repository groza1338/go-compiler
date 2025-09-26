%option never-interactive
%option yylineno
%option noyywrap

/* Macros */
IDENTIFIER      [A-Za-z_][A-Za-z_0-9]*
WHITE_SPACES    [ \t\r\n]

DEC_DIG         [0-9]

DEC_DIGS        {DIGIT}(_?{DIGIT})*

DEC_LIT         0|[1-9](_?{DEC_DIGS})?                 

%%


