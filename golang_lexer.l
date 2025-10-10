%option never-interactive
%option yylineno
%option noyywrap

/* Однострочный комментарий */
%x SCOMM

/* Многострочный комментарий */
%x MCOMM

/* Rune lit */
%x RUNE_LIT

/* Raw string */
%x RAW_STR

/* Interpreted string */
%x INTER_STR

/* Macros */
IDENTIFIER          [A-Za-z_][A-Za-z_0-9]*
WHITE_SPACES_NO_NL        [ \t\f\v]+
NL                        (\r\n|\n)

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
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cerrno>
using std::cout;

#define MAX_STRING_LENGTH 2048

#define SEMI_ON()  (can_insert_semi = 1)
#define SEMI_OFF() (can_insert_semi = 0)

static char comment_buf[8192];
static size_t comment_pos = 0;
static int comment_start_line = 0;

static char str_buf[MAX_STRING_LENGTH];
static int str_pos = 0;

static int can_insert_semi = 0;

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
    memset(str_buf, 0, str_pos + 1);
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

static inline std::string strip_underscores(const char* y) {
    std::string s(y);
    s.erase(std::remove(s.begin(), s.end(), '_'), s.end());
    return s;
}

static inline unsigned long long parse_u64(const char* p, int base) {
    errno = 0;
    char* end = nullptr;
    unsigned long long v = std::strtoull(p, &end, base);
    return v;
}

%}

%%

"break"                 {cout << "Keyword: " << yytext << '\n'; SEMI_ON();}
"case"                  {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"chan"                  {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"const"                 {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"continue"              {cout << "Keyword: " << yytext << '\n'; SEMI_ON();}
"default"               {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"defer"                 {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"else"                  {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"fallthrough"           {cout << "Keyword: " << yytext << '\n'; SEMI_ON();}
"for"                   {cout << "Keyword: " << yytext << '\n'; SEMI_OFF(); }
"func"                  {cout << "Keyword: " << yytext << '\n'; SEMI_OFF(); }
"go"                    {cout << "Keyword: " << yytext << '\n'; SEMI_OFF(); }
"goto"                  {cout << "Keyword: " << yytext << '\n'; SEMI_OFF(); }
"if"                    {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"import"                {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"interface"             {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"map"                   {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"package"               {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"range"                 {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"return"                {cout << "Keyword: " << yytext << '\n'; SEMI_ON();}
"select"                {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"struct"                {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"switch"                {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"type"                  {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}
"var"                   {cout << "Keyword: " << yytext << '\n'; SEMI_OFF();}

{IDENTIFIER}            { cout << "Identifier: " << yytext << '\n'; SEMI_ON();}

"<<="                   { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
">>="                   { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"&^="                   { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}

"..."                   { cout << "Varargs: " << yytext << '\n'; SEMI_OFF();}


"<<"                    { cout << "Operator: " << yytext << " (shl)\n"; SEMI_OFF();}
">>"                    { cout << "Operator: " << yytext << " (shr)\n"; SEMI_OFF();}
"&^"                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"&="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"|="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"^="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"&&"                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"||"                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"<-"                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"++"                    { cout << "Operator: " << yytext << '\n'; SEMI_ON();}
"--"                    { cout << "Operator: " << yytext << '\n'; SEMI_ON();}
"=="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}

"+="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"-="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"*="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"/="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"%="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"!="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
">="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"<="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
":="                    { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}

"+"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"-"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"*"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"/"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"%"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"&"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"|"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"^"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"<"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
">"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"="                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"!"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"~"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
"."                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
","                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
";"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}
":"                     { cout << "Operator: " << yytext << '\n'; SEMI_OFF();}


"("                     { cout << "OParen: "  << yytext << '\n'; SEMI_OFF();}
")"                     { cout << "CParen: "  << yytext << '\n'; SEMI_ON();}
"["                     { cout << "OBracket: "<< yytext << '\n'; SEMI_OFF();}
"]"                     { cout << "CBracket: "<< yytext << '\n'; SEMI_ON();}
"{"                     { cout << "OBrace: "  << yytext << '\n'; SEMI_OFF();}
"}"                     { cout << "CBrace: "  << yytext << '\n'; SEMI_ON();}

{DEC_LIT} {
    std::string s = strip_underscores(yytext);
    unsigned long long v = parse_u64(s.c_str(), 10);
    cout << "Decimal integer: " << v << '\n'; SEMI_ON();
}
{OCT_LIT} {
    std::string s = strip_underscores(yytext);
    const char* p = s.c_str();
    if (s.size() >= 2 && (s[1]=='o' || s[1]=='O'))
        p += 2;
    unsigned long long v = parse_u64(p, 8);
    cout << "Octal integer: " << v << '\n'; SEMI_ON();
}
{HEX_LIT} {
    std::string s = strip_underscores(yytext);
    const char* p = s.c_str() + 2;
    unsigned long long v = parse_u64(p, 16);
    cout << "Hexadecimal integer: " << v << '\n'; SEMI_ON();
}
{DEC_FLOAT_LIT} {
    std::string s = strip_underscores(yytext);
    double d = std::stod(s);
    cout << "Decimal float: " << d << '\n'; SEMI_ON();
}

"//"                    { BEGIN(SCOMM); }
<SCOMM>[^\r\n]*         { cout << "Single-line comment: " << yytext << '\n'; BEGIN(INITIAL); }
<SCOMM><<EOF>>          { BEGIN(INITIAL); }

"/*"                    { comment_start_line = yylineno; BEGIN(MCOMM); }
<MCOMM>{NL} {
    if (can_insert_semi) {
        cout << "Semicolon: ; (inserted)\n";
        SEMI_OFF();
    }
    comm_append("\n", 1);
}
<MCOMM>[^*\r\n]*        { comm_append(yytext, yyleng); }
<MCOMM>"*"+[^*/\r\n]*   { comm_append(yytext, yyleng); }
<MCOMM>"*"+"/"          { cout << "Multi-line comment: " << comment_buf << '\n'; comm_reset(); BEGIN(INITIAL); }
<MCOMM><<EOF>>          { cout << "ERROR: unterminated multiline comment (opened at line " << comment_start_line << ")\n"; comm_reset(); BEGIN(INITIAL); }

\"                          { BEGIN(INTER_STR); }
\'							{ BEGIN(RUNE_LIT); }

<INTER_STR>[^\\\"\n]+       { str_add_chars(yytext, yyleng); }
<RUNE_LIT>[^\\\'\n]+       { str_add_chars(yytext, yyleng); }
<INTER_STR,RUNE_LIT>\\a              { str_add_char('\a'); }
<INTER_STR,RUNE_LIT>\\b              { str_add_char('\b'); }
<INTER_STR,RUNE_LIT>\\f              { str_add_char('\f'); }
<INTER_STR,RUNE_LIT>\\n              { str_add_char('\n'); }
<INTER_STR,RUNE_LIT>\\r              { str_add_char('\r'); }
<INTER_STR,RUNE_LIT>\\t              { str_add_char('\t'); }
<INTER_STR,RUNE_LIT>\\v              { str_add_char('\v'); }
<INTER_STR,RUNE_LIT>\\\\             { str_add_char('\\'); }
<INTER_STR,RUNE_LIT>\\\"             { str_add_char('"'); }
<INTER_STR,RUNE_LIT>\\\'             { str_add_char('\''); }
<INTER_STR,RUNE_LIT>{OCT_BYTE_VAL}   { octal_escape(yytext + 1); }
<INTER_STR,RUNE_LIT>{HEX_BYTE_VAL}   { hex_escape(yytext + 2); }
<INTER_STR,RUNE_LIT>{LITTLE_U_VAL}   { from_unicode_to_utf8(yytext + 2); }
<INTER_STR,RUNE_LIT>{BIG_U_VAL}      { from_unicode_to_utf8(yytext + 2); }
<INTER_STR,RUNE_LIT>\n               { cout << "ERROR: newline in interpreted string literal\n"; str_reset(); BEGIN(INITIAL); }
<INTER_STR,RUNE_LIT><<EOF>>          { cout << "ERROR: unterminated string (\") before EOF\n"; BEGIN(INITIAL); }

<INTER_STR>\"               { cout << "Interpreted string: " << str_buf << '\n'; str_reset(); SEMI_ON(); BEGIN(INITIAL); }
<RUNE_LIT>\'				{ 
	if (strlen(str_buf) <= 1) { 
		cout << "Rune literal: " << str_buf << '\n'; SEMI_ON();
	} else {
		cout << "ERROR: more then one char in rune literal: " << str_buf << "\n";
	} 
	str_reset(); 
	BEGIN(INITIAL); 
}


`                 			{ BEGIN(RAW_STR); }
<RAW_STR>[^`\r]+           	{ str_add_chars(yytext, yyleng); }
<RAW_STR>\r                	{ ; }
<RAW_STR>`                 	{ cout << "Raw string: " << str_buf << '\n'; str_reset(); SEMI_ON(); BEGIN(INITIAL); }
<RAW_STR><<EOF>>          	{ cout << "ERROR: unterminated string (`) before EOF\n"; BEGIN(INITIAL); }

{NL}                        { if (can_insert_semi) { cout << "Semicolon: ; (inserted)\n"; can_insert_semi = 0; } }
{WHITE_SPACES_NO_NL}        { ; }

<<EOF>> { if (can_insert_semi) { cout << "Semicolon: ; (inserted before EOF)\n"; } return 0; }


%%