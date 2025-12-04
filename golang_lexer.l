%option never-interactive
%option yylineno
%option noyywrap

/* Однострочный комментарий */
%x SCOMM

/* Многострочный комментарий */
%x MCOMM

/* Rune lit */
%x RUNE_STATE

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
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <string>
#include <cctype>

#include "golang_parser.hpp"

using std::cout;

#define MAX_STRING_LENGTH 2048

void str_add_char(char *dest_str, unsigned char c);
void str_add_chars(char *dest_str, const char *src_str, size_t len);
void str_reset(char *str);
void comm_reset(char *str, int *start_line);
void from_unicode_to_utf8(char *dest_str, char *unicode_str);
void hex_escape(char *dest_str, const char *hex);
void octal_escape(char *dest_str, const char *oct);
std::string strip_underscores(const char* y);
unsigned long long parse_u64(const char* p, int base);
void set_semi_on(int *flag);
void set_semi_off(int *flag);

%}

%%

%{
char comment_buf[MAX_STRING_LENGTH] = {0};
int comment_start_line = -1;
char str_buf[MAX_STRING_LENGTH] = {0};
static int can_insert_semi = 0;
%}

"break"         {cout << "Keyword: " << yytext << '\n'; set_semi_on(&can_insert_semi); return BREAK;}
"case"          {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return CASE;}
"const"         {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return CONST;}
"continue"      {cout << "Keyword: " << yytext << '\n'; set_semi_on(&can_insert_semi); return CONTINUE;}
"default"       {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return DEFAULT;}
"else"          {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return ELSE;}
"for"           {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return FOR;}
"func"          {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return FUNC;}
"if"            {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return IF;}
"import"        {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return IMPORT;}
"package"       {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return PACKAGE;}
"range"         {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return RANGE;}
"return"        {cout << "Keyword: " << yytext << '\n'; set_semi_on(&can_insert_semi); return RETURN;}
"switch"        {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return SWITCH;}
"var"           {cout << "Keyword: " << yytext << '\n'; set_semi_off(&can_insert_semi); return VAR;}
"int64"         {cout << "Type: " << yytext << '\n'; set_semi_off(&can_insert_semi); return INT;}
"float64"       {cout << "Type: " << yytext << '\n'; set_semi_off(&can_insert_semi); return FLOAT;}
"bool"          {cout << "Type: " << yytext << '\n'; set_semi_off(&can_insert_semi); return BOOL;}
"string"        {cout << "Type: " << yytext << '\n'; set_semi_off(&can_insert_semi); return STRING;}
"rune"          {cout << "Type: " << yytext << '\n'; set_semi_off(&can_insert_semi); return RUNE;}
"true"          {cout << "Bool literal: " << yytext << '\n'; yylval.int_lit = 1; set_semi_on(&can_insert_semi); return BOOL_LIT;}
"false"         {cout << "Bool literal: " << yytext << '\n'; yylval.int_lit = 0; set_semi_on(&can_insert_semi); return BOOL_LIT;}

{IDENTIFIER}    { cout << "Identifier: " << yytext << '\n'; yylval.str_lit = new std::string(yytext); set_semi_on(&can_insert_semi); return ID;}

"&&"            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return AND;}
"||"            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return OR;}
"++"            { cout << "Operator: " << yytext << '\n'; set_semi_on(&can_insert_semi); return INC;}
"--"            { cout << "Operator: " << yytext << '\n'; set_semi_on(&can_insert_semi); return DEC;}
"=="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return EQUAL;}

"+="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi);}
"-="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi);}
"*="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi);}
"/="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi);}
"%="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi);}
"!="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return NEQUAL;}
">="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return GREATER_EQUAL;}
"<="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return LESS_EQUAL;}
":="            { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return WALRUS;}

"+"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '+';}
"-"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '-';}
"*"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '*';}
"/"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '/';}
"<"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '<';}
">"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '>';}
"="             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '=';}
"!"             { cout << "Operator: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '!';}

"("             { cout << "OParen: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '(';}
")"             { cout << "CParen: " << yytext << '\n'; set_semi_on(&can_insert_semi); return ')';}
"["             { cout << "OBracket: "<< yytext << '\n'; set_semi_off(&can_insert_semi); return '[';}
"]"             { cout << "CBracket: "<< yytext << '\n'; set_semi_on(&can_insert_semi); return ']';}
"{"             { cout << "OBrace: " << yytext << '\n'; set_semi_off(&can_insert_semi); return '{';}
"}"             { cout << "CBrace: " << yytext << '\n'; set_semi_on(&can_insert_semi); return '}';}

{DEC_LIT} {
    std::string s = strip_underscores(yytext);
    unsigned long long v = parse_u64(s.c_str(), 10);
    cout << "Decimal integer: " << v << '\n';
    yylval.int_lit = v;
    set_semi_on(&can_insert_semi);
    return INT_LIT;
}
{OCT_LIT} {
    std::string s = strip_underscores(yytext);
    const char* p = s.c_str();
    if (s.size() >= 2 && (s[1]=='o' || s[1]=='O'))
        p += 2;
    unsigned long long v = parse_u64(p, 8);
    cout << "Octal integer: " << v << '\n';
    yylval.int_lit = v;
    set_semi_on(&can_insert_semi);
    return INT_LIT;
}
{HEX_LIT} {
    std::string s = strip_underscores(yytext);
    const char* p = s.c_str() + 2;
    unsigned long long v = parse_u64(p, 16);
    cout << "Hexadecimal integer: " << v << '\n';
    yylval.int_lit = v;
    set_semi_on(&can_insert_semi);
    return INT_LIT;
}
{DEC_FLOAT_LIT} {
    std::string s = strip_underscores(yytext);
    double d = std::stod(s);
    cout << "Decimal float: " << d << '\n';
    yylval.float_lit = d;
    set_semi_on(&can_insert_semi);
    return FLOAT_LIT;
}

"//"                                    { BEGIN(SCOMM); }
<SCOMM>[^\r\n]*                         { cout << "Single-line comment: " << yytext << '\n'; BEGIN(INITIAL); }
<SCOMM><<EOF>>                          { BEGIN(INITIAL); }

"/*"                                    { comment_start_line = yylineno; BEGIN(MCOMM); }
<MCOMM>{NL}                             { ; }
<MCOMM>[^*\r\n]*                        { str_add_chars(comment_buf, yytext, yyleng); }
<MCOMM>"*"+[^*/\r\n]*                   { str_add_chars(comment_buf, yytext, yyleng); }
<MCOMM>"*"+"/"                          { cout << "Multi-line comment: " << comment_buf << '\n'; comm_reset(comment_buf, &comment_start_line); BEGIN(INITIAL); }
<MCOMM><<EOF>>                          { cout << "ERROR: unterminated multiline comment (opened at line " << comment_start_line << ")\n"; comm_reset(comment_buf, &comment_start_line); BEGIN(INITIAL); }

\"                                      { str_reset(str_buf); BEGIN(INTER_STR); }
\'							            { str_reset(str_buf); BEGIN(RUNE_STATE); }

<INTER_STR>[^\\\"\n]+       			{ str_add_chars(str_buf, yytext, yyleng); }
<RUNE_STATE>[^\\\'\n]+       				{ str_add_chars(str_buf, yytext, yyleng); }
<INTER_STR,RUNE_STATE>\\a              	{ str_add_char(str_buf, '\a'); }
<INTER_STR,RUNE_STATE>\\b              	{ str_add_char(str_buf, '\b'); }
<INTER_STR,RUNE_STATE>\\f              	{ str_add_char(str_buf, '\f'); }
<INTER_STR,RUNE_STATE>\\n              	{ str_add_char(str_buf, '\n'); }
<INTER_STR,RUNE_STATE>\\r              	{ str_add_char(str_buf, '\r'); }
<INTER_STR,RUNE_STATE>\\t              	{ str_add_char(str_buf, '\t'); }
<INTER_STR,RUNE_STATE>\\v                 { str_add_char(str_buf, '\v'); }
<INTER_STR,RUNE_STATE>\\\\                { str_add_char(str_buf, '\\'); }
<INTER_STR,RUNE_STATE>\\\"                { str_add_char(str_buf, '"'); }
<INTER_STR,RUNE_STATE>\\\'                { str_add_char(str_buf, '\''); }
<INTER_STR,RUNE_STATE>{OCT_BYTE_VAL}      { octal_escape(str_buf, yytext + 1); }
<INTER_STR,RUNE_STATE>{HEX_BYTE_VAL}      { hex_escape(str_buf, yytext + 2); }
<INTER_STR,RUNE_STATE>{LITTLE_U_VAL}      { from_unicode_to_utf8(str_buf, yytext + 2); }
<INTER_STR,RUNE_STATE>{BIG_U_VAL}         { from_unicode_to_utf8(str_buf, yytext + 2); }
<INTER_STR,RUNE_STATE>\n                  { cout << "ERROR: newline in interpreted string literal\n"; str_reset(str_buf); BEGIN(INITIAL); }
<INTER_STR,RUNE_STATE><<EOF>>             { cout << "ERROR: unterminated string (\") before EOF\n"; BEGIN(INITIAL); }

<INTER_STR>\" {
    cout << "Interpreted string: " << str_buf << '\n';
    yylval.str_lit = new std::string(str_buf);
    str_reset(str_buf);
    set_semi_on(&can_insert_semi);
    BEGIN(INITIAL);
    return STRING_LIT;
}

<RUNE_STATE>\' {
    if (strlen(str_buf) <= 1) {
        cout << "Rune literal: " << str_buf << '\n';
        yylval.str_lit = new std::string(str_buf);
        set_semi_on(&can_insert_semi);
        str_reset(str_buf);
        BEGIN(INITIAL);
        return RUNE_LIT;
    } else {
        cout << "ERROR: more then one char in rune literal: " << str_buf << "\n";
        str_reset(str_buf);
        BEGIN(INITIAL);
    }
}

`                           { str_reset(str_buf); BEGIN(RAW_STR); }
<RAW_STR>[^`\r]+            { str_add_chars(str_buf, yytext, yyleng); }
<RAW_STR>\r                 { ; }
<RAW_STR>` {
    cout << "Raw string: " << str_buf << '\n';
    yylval.str_lit = new std::string(str_buf);
    str_reset(str_buf);
    set_semi_on(&can_insert_semi);
    BEGIN(INITIAL);
    return STRING_LIT;
}

<RAW_STR><<EOF>> {
    cout << "ERROR: unterminated string (`) before EOF\n";
    BEGIN(INITIAL);
}

;   {
    if (can_insert_semi) {
        cout << "Semicolon: ;\n";
        can_insert_semi = 0;
        return ';';
    } else {
        cout << "ERROR: Semicolon can`t placed there.";
        return 1;
    }
}

{NL} {
    if (can_insert_semi) {
        cout << "Semicolon: ; (inserted)\n";
        can_insert_semi = 0;
        return ';';
    }
}

{WHITE_SPACES_NO_NL}        { ; }

<<EOF>> {
    if (can_insert_semi) {
        cout << "Semicolon: ; (inserted before EOF)\n";
    }
    return 0;
}


%%
void str_add_char(char *dest_str, unsigned char c) {
	int dest_len = strlen(dest_str);
    if (dest_len < MAX_STRING_LENGTH - 1) {
        char tmp_buf[2];
        tmp_buf[0] = static_cast<char>(c);
        tmp_buf[1] = '\0';
        strcat(dest_str, tmp_buf);
    }
}

void str_add_chars(char *dest_str, const char *src_str, size_t len) {
	int dest_len = strlen(dest_str);
    if (dest_len + len >= MAX_STRING_LENGTH) {
		len = MAX_STRING_LENGTH - dest_len - 1;
    }
	strncat(dest_str, src_str, len);
}

void str_reset(char *str) {
    *str = '\0';
}

void comm_reset(char *str, int *start_line) {
    str_reset(str);
    *start_line = -1;
}

void from_unicode_to_utf8(char *dest_str, char *unicode_str) {
    int code_point = strtol(unicode_str, NULL, 16);
    if (code_point <= 0x7F) {
        str_add_char(dest_str, (char)code_point);
    } else if (code_point <= 0x7FF) {
        str_add_char(dest_str, 0xC0 | ((code_point >> 6) & 0x1F));
        str_add_char(dest_str, 0x80 | (code_point & 0x3F));
    } else if (code_point <= 0xFFFF) {
        str_add_char(dest_str, 0xE0 | ((code_point >> 12) & 0x0F));
        str_add_char(dest_str, 0x80 | ((code_point >> 6) & 0x3F));
        str_add_char(dest_str, 0x80 | (code_point & 0x3F));
    } else if (code_point <= 0x10FFFF) {
        str_add_char(dest_str, 0xF0 | ((code_point >> 18) & 0x07));
        str_add_char(dest_str, 0x80 | ((code_point >> 12) & 0x3F));
        str_add_char(dest_str, 0x80 | ((code_point >> 6) & 0x3F));
        str_add_char(dest_str, 0x80 | (code_point & 0x3F));
    }
}

void hex_escape(char *dest_str, const char *hex) {
    int value = strtol(hex, NULL, 16);
    str_add_char(dest_str, (char)value);
}

void octal_escape(char *dest_str, const char *oct) {
    int value = strtol(oct, NULL, 8);
    str_add_char(dest_str, (char)value);
}

std::string strip_underscores(const char* y) {
    std::string s(y);
    s.erase(std::remove(s.begin(), s.end(), '_'), s.end());
    return s;
}

unsigned long long parse_u64(const char* p, int base) {
    errno = 0;
    char* end = nullptr;
    unsigned long long v = std::strtoull(p, &end, base);
    return v;
}

void set_semi_on(int *flag) {
	*flag = 1;
}

void set_semi_off(int *flag) {
	*flag = 0;
}
