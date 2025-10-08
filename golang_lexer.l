%option never-interactive
%option yylineno
%option noyywrap

/* Однострочный комментарий */
%x SCOMM

/* Многострочный комментарий */
%x MCOMM

/* Rune */
%x RUNE_STR

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
OCT_LIT             0[oO]?_?{OCT_DIGS}

DEC_EXP             [eE][+-]?{DEC_DIGS}
DEC_FLOAT_LIT       {DEC_DIGS}\.{DEC_DIGS}?{DEC_EXP}?|{DEC_DIGS}{DEC_EXP}|\.{DEC_DIGS}{DEC_EXP}?

HEX_DIG             [0-9A-Fa-f]
HEX_DIGS            {HEX_DIG}(_?{HEX_DIG})*
HEX_LIT             0[xX]_?{HEX_DIGS}

ESC_CHAR            \\[abfnrtv\\'"]
BIG_U_VAL           \\U{HEX_DIG}{8}
LITTLE_U_VAL        \\u{HEX_DIG}{4}
HEX_BYTE_VAL        \\x{HEX_DIG}{2}
OCT_BYTE_VAL        \\{OCT_DIG}{3}
BYTE_VAL            {OCT_BYTE_VAL}|{HEX_BYTE_VAL}
UNICODE_VAL         {LITTLE_U_VAL}|{BIG_U_VAL}|{ESC_CHAR}
RUNE_LIT_BODY       ({UNICODE_VAL}|{BYTE_VAL})

INTER_STR_LIT_BODY  ({UNICODE_VAL}|{BYTE_VAL})*

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
using std::cout;

#define MAX_STRING_LENGTH 2048

static char comment_buf[8192];
static size_t comment_pos = 0;
static int comment_start_line = 0;

static char str_buf[MAX_STRING_LENGTH];
static int str_pos = 0;

static inline void str_add_char(unsigned char c) {
    if (str_pos < MAX_STRING_LENGTH - 1) {
        str_buf[str_pos++] = c;
    }
}

static inline void str_add_chars(const char *str, size_t len) {
    if (str_pos + len < MAX_STRING_LENGTH) {
        memcpy(&str_buf[str_pos], str, len);
        str_pos += len;
    } else {
        size_t available_space = MAX_STRING_LENGTH - str_pos;
        memcpy(&str_buf[str_pos], str, available_space);
        str_pos = MAX_STRING_LENGTH - 1;
    }
}

static inline void str_reset() {
    str_pos = 0;
}

static inline void from_unicode_to_utf8(char *unicode_str) {
    int code_point = strtol(unicode_str, NULL, 16);
    if (code_point <= 0x7F) {
        str_add_char((char)code_point);
    } else if (code_point <= 0x7FF) {
        str_add_char(0xC0 | ((code_point >> 6) & 0x1F));
        str_add_char(0x80 | (code_point & 0x3F));
    } else if (code_point <= 0xFFFF) {
        str_add_char(0xE0 | ((code_point >> 12) & 0x0F));
        str_add_char(0x80 | ((code_point >> 6) & 0x3F));
        str_add_char(0x80 | (code_point & 0x3F));
    } else if (code_point <= 0x10FFFF) {
        str_add_char(0xF0 | ((code_point >> 18) & 0x07));
        str_add_char(0x80 | ((code_point >> 12) & 0x3F));
        str_add_char(0x80 | ((code_point >> 6) & 0x3F));
        str_add_char(0x80 | (code_point & 0x3F));
    }
}

void hex_escape(const char *hex) {
    int value = strtol(hex, NULL, 16);
    str_add_char((char)value);
}

void octal_escape(const char *oct) {
    int value = strtol(oct, NULL, 8);
    str_add_char((char)value);
}

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

\"                          { BEGIN(INTER_STR); }
<INTER_STR>[^\\\"\n]+       { str_add_chars(yytext, yyleng); }
<INTER_STR>\\a              { str_add_char('\a'); }
<INTER_STR>\\b              { str_add_char('\b'); }
<INTER_STR>\\f              { str_add_char('\f'); }
<INTER_STR>\\n              { str_add_char('\n'); }
<INTER_STR>\\r              { str_add_char('\r'); }
<INTER_STR>\\t              { str_add_char('\t'); }
<INTER_STR>\\v              { str_add_char('\v'); }
<INTER_STR>\\\\             { str_add_char('\\'); }
<INTER_STR>\\\"             { str_add_char('"'); }
<INTER_STR>\\\'             { str_add_char('\''); }
<INTER_STR>{OCT_BYTE_VAL}   { octal_escape(yytext + 1); }
<INTER_STR>{HEX_BYTE_VAL}   { hex_escape(yytext + 2); }
<INTER_STR>{LITTLE_U_VAL}   { from_unicode_to_utf8(yytext + 2); }
<INTER_STR>{BIG_U_VAL}      { from_unicode_to_utf8(yytext + 2); }
<INTER_STR>\n               { cout << "ERROR: newline in interpreted string literal\n"; str_reset(); BEGIN(INITIAL); }
<INTER_STR>\"               { cout << "Interpreted string: " << str_buf << '\n'; str_reset(); BEGIN(INITIAL); }
<INTER_STR><<EOF>>          { cout << "ERROR: unterminated string (\") before EOF\n"; BEGIN(INITIAL); }


"`"                     { BEGIN(RAW_STR); }
<RAW_STR>[^`\r\n]       { str_add_chars(yytext, yyleng); }
<RAW_STR>\n             { str_add_chars("\n", 1); }
<RAW_STR>"`"            { cout << "Raw string: " << str_buf << '\n'; str_reset(); BEGIN(INITIAL); }

{WHITE_SPACES}          { ; }

%%