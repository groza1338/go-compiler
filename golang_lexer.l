%option never-interactive
%option yylineno
%option noyywrap

/* Однострочный комментарий */
%x SCOMM
/* Многострочный комментарий */
%x MCOMM

/* Macros */
IDENTIFIER          [A-Za-z_][A-Za-z_0-9]*
WHITE_SPACES        [ \t\r\n]

DEC_DIG             [0-9]
DEC_DIGS            {DEC_DIG}(_?{DEC_DIG})*
DEC_LIT             0|[1-9](_?{DEC_DIGS})?

OCT_DIG             [0-7]

DEC_EXP             [eE][+-]?{DEC_DIGS}
DEC_FLOAT_LIT       {DEC_DIGS}\.{DEC_DIGS}?{DEC_EXP}?|{DEC_DIGS}{DEC_EXP}|\.{DEC_DIGS}{DEC_EXP}?

HEX_DIG             [0-9A-Fa-f]

ESC_CHAR            \\[abfnrtv\\'"]
BIG_U_VAL           \\U{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}
LITTLE_U_VAL        \\u{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}
HEX_BYTE_VAL        \\x{HEX_DIG}{HEX_DIG}
OCT_BYTE_VAL        \\{OCT_DIG}{OCT_DIG}{OCT_DIG}
BYTE_VAL            {OCT_BYTE_VAL}|{HEX_BYTE_VAL}
UNICODE_VAL         {LITTLE_U_VAL}|{BIG_U_VAL}|{ESC_CHAR}
RUNE_LIT            '({UNICODE_VAL}|{BYTE_VAL})'

%{
#include <iostream>
using std::cout;
%}

%%

"append"                {cout << "Predeclared Functions: " << yytext << '\n';}
"cap"                   {cout << "Predeclared Functions: " << yytext << '\n';}
"clear"                 {cout << "Predeclared Functions: " << yytext << '\n';}
"close"                 {cout << "Predeclared Functions: " << yytext << '\n';}
"complex"               {cout << "Predeclared Functions: " << yytext << '\n';}
"copy"                  {cout << "Predeclared Functions: " << yytext << '\n';}
"delete"                {cout << "Predeclared Functions: " << yytext << '\n';}
"imag"                  {cout << "Predeclared Functions: " << yytext << '\n';}
"len"                   {cout << "Predeclared Functions: " << yytext << '\n';}
"make"                  {cout << "Predeclared Functions: " << yytext << '\n';}
"max"                   {cout << "Predeclared Functions: " << yytext << '\n';}
"min"                   {cout << "Predeclared Functions: " << yytext << '\n';}
"new"                   {cout << "Predeclared Functions: " << yytext << '\n';}
"panic"                 {cout << "Predeclared Functions: " << yytext << '\n';}
"println"               {cout << "Predeclared Functions: " << yytext << '\n';}
"print"                 {cout << "Predeclared Functions: " << yytext << '\n';}
"real"                  {cout << "Predeclared Functions: " << yytext << '\n';}
"recover"               {cout << "Predeclared Functions: " << yytext << '\n';}


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

"true"                  { cout << "Predeclared Constants: " << yytext << '\n'; }
"false"                 { cout << "Predeclared Constants: " << yytext << '\n'; }
"iota"                  { cout << "Predeclared Constants: " << yytext << '\n'; }

"iota"                  { cout << "Predeclared Zero value: " << yytext << '\n'; }

"any"                   { cout << "Predeclared Types: " << yytext << '\n'; }
"bool"                  { cout << "Predeclared Types: " << yytext << '\n'; }
"byte"                  { cout << "Predeclared Types: " << yytext << '\n'; }
"comparable"            { cout << "Predeclared Types: " << yytext << '\n'; }
"complex64"             { cout << "Predeclared Types: " << yytext << '\n'; }
"complex128"            { cout << "Predeclared Types: " << yytext << '\n'; }
"error"                 { cout << "Predeclared Types: " << yytext << '\n'; }
"float32"               { cout << "Predeclared Types: " << yytext << '\n'; }
"float64"               { cout << "Predeclared Types: " << yytext << '\n'; }
"int"                   { cout << "Predeclared Types: " << yytext << '\n'; }
"int8"                  { cout << "Predeclared Types: " << yytext << '\n'; }
"int16"                 { cout << "Predeclared Types: " << yytext << '\n'; }
"int32"                 { cout << "Predeclared Types: " << yytext << '\n'; }
"int64"                 { cout << "Predeclared Types: " << yytext << '\n'; }
"rune"                  { cout << "Predeclared Types: " << yytext << '\n'; }
"string"                { cout << "Predeclared Types: " << yytext << '\n'; }
"uint"                  { cout << "Predeclared Types: " << yytext << '\n'; }
"uint8"                 { cout << "Predeclared Types: " << yytext << '\n'; }
"uint16"                { cout << "Predeclared Types: " << yytext << '\n'; }
"uint32"                { cout << "Predeclared Types: " << yytext << '\n'; }
"uint64"                { cout << "Predeclared Types: " << yytext << '\n'; }
"uintptr"               { cout << "Predeclared Types: " << yytext << '\n'; }

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