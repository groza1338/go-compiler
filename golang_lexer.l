%option never-interactive
%option yylineno
%option noyywrap

/* Однострочный комментарий */
%x SCOMM

/* Многострочный комментарий */
%x MCOMM

/* Rune */
%x RUNE_STR
%x RUNE_QUOTE_WAIT
%x SKIP_QUOTE

/* Raw string */
%x RAW_STR

/* Interpreted string */
%x INTER_STR

/* Macros */
IDENTIFIER          [A-Za-z_][A-Za-z_0-9]*
WHITE_SPACES        [ \t\r\n]

DEC_DIG             [0-9]
DEC_DIGS            {DEC_DIG}(_?{DEC_DIG})*
DEC_LIT             0|[1-9](_?{DEC_DIGS})?

OCT_DIG             [0-7]
OCT_DIGS            {OCT_DIG}(_?{OCT_DIG})*
OCT_LIT             0[oO]_?{OCT_DIGS}

DEC_EXP             [eE][+-]?{DEC_DIGS}
DEC_FLOAT_LIT       {DEC_DIGS}\.{DEC_DIGS}?{DEC_EXP}?|{DEC_DIGS}{DEC_EXP}|\.{DEC_DIGS}{DEC_EXP}?

HEX_DIG             [0-9A-Fa-f]
HEX_DIGS            {HEX_DIG}(_?{HEX_DIG})*
HEX_LIT             0[xX]_?{HEX_DIGS}

ESC_CHAR            \\[abfnrtv\\'"]
BIG_U_VAL           \\U{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}
LITTLE_U_VAL        \\u{HEX_DIG}{HEX_DIG}{HEX_DIG}{HEX_DIG}
HEX_BYTE_VAL        \\x{HEX_DIG}{HEX_DIG}
OCT_BYTE_VAL        \\{OCT_DIG}{OCT_DIG}{OCT_DIG}
BYTE_VAL            {OCT_BYTE_VAL}|{HEX_BYTE_VAL}
UNICODE_VAL         {LITTLE_U_VAL}|{BIG_U_VAL}|{ESC_CHAR}
RUNE_LIT_BODY       ({UNICODE_VAL}|{BYTE_VAL})

INTER_STR_LIT_BODY  ({UNICODE_VAL}|{BYTE_VAL})*

%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
using std::cout;

static char comment_buf[8192];
static size_t comment_pos = 0;
static int comment_start_line = 0;

static char str_buf[8192];
static size_t str_pos = 0;

static inline void comm_append(const char* s, size_t n) {
    if (comment_pos + n >= sizeof(comment_buf)) n = sizeof(comment_buf) - 1 - comment_pos;
    if (n > 0) {
        std::memcpy(comment_buf + comment_pos, s, n);
        comment_pos += n;
        comment_buf[comment_pos] = '\0';
    }
}

static inline void comm_reset() {
    comment_pos = 0;
    comment_buf[0] = '\0';
    comment_start_line = 0;
}

static inline void str_append(const char* s, size_t n) {
    if (str_pos + n >= sizeof(str_buf)) n = sizeof(str_buf) - 1 - str_pos;
    if (n > 0) {
        std::memcpy(str_buf + str_pos, s, n);
        str_pos += n;
        str_buf[str_pos] = '\0';
    }
}

static inline void str_reset() {
    str_pos = 0;
    str_buf[0] = '\0';
}

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

{IDENTIFIER}            { cout << "Identifier: " << yytext << '\n'; }

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

{DEC_LIT}               { cout << "Decimal integer: " << std::stoi(yytext) << '\n'; }
{OCT_LIT}               { cout << "Octal integer: " << std::stoi(yytext, nullptr, 8) << '\n'; }
{HEX_LIT}               { cout << "Hexadecimal integer: " << std::stoi(yytext, nullptr, 16) << '\n'; }
{DEC_FLOAT_LIT}         { cout << "Decimal float: " << std::stod(yytext) << '\n'; }

"//"                    { BEGIN(SCOMM); }
<SCOMM>[^\r\n]*         { cout << "Single-line comment: " << '"' << yytext << '"' << '\n'; BEGIN(INITIAL); }
<SCOMM><<EOF>>          { BEGIN(INITIAL); }

"/*"                    { comment_start_line = yylineno; BEGIN(MCOMM); }
<MCOMM>\r\n             { comm_append("\n", 1); }
<MCOMM>[^*\r\n]*        { comm_append(yytext, yyleng); }
<MCOMM>"*"+[^*/\r\n]*   { comm_append(yytext, yyleng); }
<MCOMM>"*"+"/"          { cout << "Multi-line comment: " << comment_buf << '\n'; comm_reset(); BEGIN(INITIAL); }
<MCOMM><<EOF>>          { cout << "ERROR: unterminated multiline comment (opened at line " << comment_start_line << ")\n"; comm_reset(); BEGIN(INITIAL); }

\"                      { BEGIN(INTER_STR); }
<INTER_STR>\"           { cout << "Interpreted string: " << str_buf << '\n'; str_reset(); BEGIN(INITIAL); }
<INTER_STR>[^\\\"\n]+   { str_append(yytext, yyleng); }
<INTER_STR>{ESC_CHAR}   {
    switch (yytext[1]) {
        case 'a': str_append("\a", 1); break;
        case 'b': str_append("\b", 1); break;
        case 'f': str_append("\f", 1); break;
        case 'n': str_append("\n", 1); break;
        case 'r': str_append("\r", 1); break;
        case 't': str_append("\t", 1); break;
        case 'v': str_append("\v", 1); break;
        case '\\': str_append("\\", 1); break;
        case '"': str_append("\"", 1); break;
        case '\'': str_append("\'", 1); break;
    }
}
<INTER_STR>{BYTE_VAL} {
    int val = std::stoi(yytext + 2, nullptr, (yytext[1] == 'x') ? 16 : 8);
    str_append((const char*)&val, 1);
}
<INTER_STR>{LITTLE_U_VAL} {
    int code = std::stoi(yytext + 2, nullptr, 16);
    if (code <= 0x7F) {
        str_append((const char*)&code, 1);
    } else if (code <= 0x7FF) {
        char utf8[2] = { 0xC0 | ((code >> 6) & 0x1F), 0x80 | (code & 0x3F) };
        str_append(utf8, 2);
    } else {
        char utf8[3] = { 0xE0 | ((code >> 12) & 0x0F), 0x80 | ((code >> 6) & 0x3F), 0x80 | (code & 0x3F) };
        str_append(utf8, 3);
    }
}
<INTER_STR>{BIG_U_VAL} {
    int code = std::stoi(yytext + 2, nullptr, 16);
    if (code <= 0x7F) {
        str_append((const char*)&code, 1);
    } else if (code <= 0x7FF) {
        char utf8[2] = { 0xC0 | ((code >> 6) & 0x1F), 0x80 | (code & 0x3F) };
        str_append(utf8, 2);
    } else if (code <= 0xFFFF) {
        char utf8[3] = { 0xE0 | ((code >> 12) & 0x0F), 0x80 | ((code >> 6) & 0x3F), 0x80 | (code & 0x3F) };
        str_append(utf8, 3);
    } else if (code <= 0x10FFFF) {
        char utf8[4] = { 0xF0 | ((code >> 18) & 0x07), 0x80 | ((code >> 12) & 0x3F), 0x80 | ((code >> 6) & 0x3F), 0x80 | (code & 0x3F) };
        str_append(utf8, 4);
    } else {
        cout << "ERROR: invalid Unicode code point (out of range)" << '\n';
    }
}
<INTER_STR>\n           { cout << "ERROR: newline in interpreted string literal\n"; str_reset(); BEGIN(INITIAL); }

\'                      { BEGIN(RUNE_STR); }
<SKIP_QUOTE>\'          { BEGIN(INITIAL); }
<RUNE_QUOTE_WAIT>\'     { cout << "Rune literal: " << str_buf << '\n'; str_reset(); BEGIN(INITIAL); }
<RUNE_QUOTE_WAIT>[^']+  { cout << "ERROR: more than one character in rune literal: " << yytext << "\n"; str_reset(); BEGIN(SKIP_QUOTE); }
<RUNE_STR>[^\\\'\n]     { str_append(yytext, yyleng); BEGIN(RUNE_QUOTE_WAIT); }
<RUNE_STR>{ESC_CHAR}    {
    switch (yytext[1]) {
        case 'a': str_append("\a", 1); break;
        case 'b': str_append("\b", 1); break;
        case 'f': str_append("\f", 1); break;
        case 'n': str_append("\n", 1); break;
        case 'r': str_append("\r", 1); break;
        case 't': str_append("\t", 1); break;
        case 'v': str_append("\v", 1); break;
        case '\\': str_append("\\", 1); break;
        case '"': str_append("\"", 1); break;
        case '\'': str_append("\'", 1); break;
    }
    BEGIN(RUNE_QUOTE_WAIT);
}
<RUNE_STR>{BYTE_VAL}    {
    int val = std::stoi(yytext + 2, nullptr, (yytext[1] == 'x') ? 16 : 8);
    str_append((const char*)&val, 1);
    BEGIN(RUNE_QUOTE_WAIT);
}
<RUNE_STR>{LITTLE_U_VAL} {
    int code = std::stoi(yytext + 2, nullptr, 16);
    if (code <= 0x7F) {
        str_append((const char*)&code, 1);
    } else if (code <= 0x7FF) {
        char utf8[2] = { 0xC0 | ((code >> 6) & 0x1F), 0x80 | (code & 0x3F) };
        str_append(utf8, 2);
    } else {
        char utf8[3] = { 0xE0 | ((code >> 12) & 0x0F), 0x80 | ((code >> 6) & 0x3F), 0x80 | (code & 0x3F) };
        str_append(utf8, 3);
    }
    BEGIN(RUNE_QUOTE_WAIT);
}
<RUNE_STR>{BIG_U_VAL} {
    int code = std::stoi(yytext + 2, nullptr, 16);
    if (code <= 0x7F) {
        str_append((const char*)&code, 1);
    } else if (code <= 0x7FF) {
        char utf8[2] = { 0xC0 | ((code >> 6) & 0x1F), 0x80 | (code & 0x3F) };
        str_append(utf8, 2);
    } else if (code <= 0xFFFF) {
        char utf8[3] = { 0xE0 | ((code >> 12) & 0x0F), 0x80 | ((code >> 6) & 0x3F), 0x80 | (code & 0x3F) };
        str_append(utf8, 3);
    } else if (code <= 0x10FFFF) {
        char utf8[4] = { 0xF0 | ((code >> 18) & 0x07), 0x80 | ((code >> 12) & 0x3F), 0x80 | ((code >> 6) & 0x3F), 0x80 | (code & 0x3F) };
        str_append(utf8, 4);
    } else {
        cout << "ERROR: invalid Unicode code point (out of range)" << '\n';
    }
    BEGIN(RUNE_QUOTE_WAIT);
}
<RUNE_STR>\n            { cout << "ERROR: newline in rune string literal\n"; str_reset(); BEGIN(INITIAL); }
<RUNE_STR>[^']+             { cout << "ERROR: unknown rune literal: " << yytext << '\n'; BEGIN(SKIP_QUOTE); }
<RUNE_STR,RUNE_QUOTE_WAIT,SKIP_QUOTE><<EOF>>       { cout << "ERROR: unterminated string (') before EOF\n"; BEGIN(INITIAL); }

"`"                     { BEGIN(RAW_STR); }
<RAW_STR>[^`\r\n]       { str_append(yytext, yyleng); }
<RAW_STR>\n             { str_append("\n", 1); }
<RAW_STR>"`"            { cout << "Raw string: " << str_buf << '\n'; str_reset(); BEGIN(INITIAL); }

{WHITE_SPACES}          { ; }

%%