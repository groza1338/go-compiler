%option never-interactive
%option yylineno
%option noyywrap

/* Однострочный комментарий */
%x SCOMM
/* Многострочный комментарий */
%x MCOMM

/* Macros */
IDENTIFIER      [A-Za-z_][A-Za-z_0-9]*
WHITE_SPACES    [ \t\r\n]

DEC_DIG         [0-9]

DEC_DIGS        {DEC_DIG}(_?{DEC_DIG})*

DEC_LIT         0|[1-9](_?{DEC_DIGS})?

%{

%}

%%

"break"                 {cout << "Keyword: " << yytext << '\n';}
"case"                  {cout << "Keyword: " << yytext << '\n';}
"chan"                  {cout << "Keyword: " << yytext << '\n';}
"const"                 {cout << "Keyword: " << yytext << '\n';}
"continue"              {cout << "Keyword: " << yytext << '\n';}
"default"               {cout << "Keyword: " << yytext << '\n';}
"defer"                 {cout << "Keyword: " << yytext << '\n';}
"else"                  {cout << "Keyword: " << yytext << '\n';}
"fallthrough"           {cout << "Keyword: " << yytext << '\n';}
"for"                   {cout << "Keyword: " << yytext << '\n';}
"func"                  {cout << "Keyword: " << yytext << '\n';}
"go"                    {cout << "Keyword: " << yytext << '\n';}
"goto"                  {cout << "Keyword: " << yytext << '\n';}
"if"                    {cout << "Keyword: " << yytext << '\n';}
"import"                {cout << "Keyword: " << yytext << '\n';}
"interface"             {cout << "Keyword: " << yytext << '\n';}
"map"                   {cout << "Keyword: " << yytext << '\n';}
"package"               {cout << "Keyword: " << yytext << '\n';}
"range"                 {cout << "Keyword: " << yytext << '\n';}
"return"                {cout << "Keyword: " << yytext << '\n';}
"select"                {cout << "Keyword: " << yytext << '\n';}
"struct"                {cout << "Keyword: " << yytext << '\n';}
"switch"                {cout << "Keyword: " << yytext << '\n';}
"type"                  {cout << "Keyword: " << yytext << '\n';}
"var"                   {cout << "Keyword: " << yytext << '\n';}

"true"                  { cout << "Boolean: " << yytext << '\n'; }
"false"                 { cout << "Boolean: " << yytext << '\n'; }

"<<="                   { cout << "Operator: " << yytext << '\n'; }
">>="                   { cout << "Operator: " << yytext << '\n'; }
"&^="                   { cout << "Operator: " << yytext << '\n'; }

"..."                   { cout << "Varargs: " << yytext << '\n'; }


"<<"                    { cout << "Operator: " << yytext << " (shl)\n"; }
">>"                    { cout << "Operator: " << yytext << " (shr)\n"; }
"&^"                    { cout << "Operator: " << yytext << '\n'; }
"&="                    { cout << "Operator: " << yytext << '\n'; }
"|="                    { cout << "Operator: " << yytext << '\n'; }
"^="                    { cout << "Operator: " << yytext << '\n'; }
"&&"                    { cout << "Operator: " << yytext << '\n'; }
"||"                    { cout << "Operator: " << yytext << '\n'; }
"<-"                    { cout << "Operator: " << yytext << '\n'; }
"++"                    { cout << "Operator: " << yytext << '\n'; }
"--"                    { cout << "Operator: " << yytext << '\n'; }
"=="                    { cout << "Operator: " << yytext << '\n'; }

"+="                    { cout << "Operator: " << yytext << '\n'; }
"-="                    { cout << "Operator: " << yytext << '\n'; }
"*="                    { cout << "Operator: " << yytext << '\n'; }
"/="                    { cout << "Operator: " << yytext << '\n'; }
"%="                    { cout << "Operator: " << yytext << '\n'; }
"!="                    { cout << "Operator: " << yytext << '\n'; }
">="                    { cout << "Operator: " << yytext << '\n'; }
"<="                    { cout << "Operator: " << yytext << '\n'; }
":="                    { cout << "Operator: " << yytext << '\n'; }

"+"                     { cout << "Operator: " << yytext << '\n'; }
"-"                     { cout << "Operator: " << yytext << '\n'; }
"*"                     { cout << "Operator: " << yytext << '\n'; }
"/"                     { cout << "Operator: " << yytext << '\n'; }
"%"                     { cout << "Operator: " << yytext << '\n'; }
"&"                     { cout << "Operator: " << yytext << '\n'; }
"|"                     { cout << "Operator: " << yytext << '\n'; }
"^"                     { cout << "Operator: " << yytext << '\n'; }
"<"                     { cout << "Operator: " << yytext << '\n'; }
">"                     { cout << "Operator: " << yytext << '\n'; }
"="                     { cout << "Operator: " << yytext << '\n'; }
"!"                     { cout << "Operator: " << yytext << '\n'; }
"~"                     { cout << "Operator: " << yytext << '\n'; }
"."                     { cout << "Operator: " << yytext << '\n'; }
","                     { cout << "Operator: " << yytext << '\n'; }
";"                     { cout << "Operator: " << yytext << '\n'; }
":"                     { cout << "Operator: " << yytext << '\n'; }


"("                     { cout << "OParen: "  << yytext << '\n'; }
")"                     { cout << "CParen: "  << yytext << '\n'; }
"["                     { cout << "OBracket: "<< yytext << '\n'; }
"]"                     { cout << "CBracket: "<< yytext << '\n'; }
"{"                     { cout << "OBrace: "  << yytext << '\n'; }
"}"                     { cout << "CBrace: "  << yytext << '\n'; }

{WHITE_SPACES};

%%