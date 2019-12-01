
DIGIT           [0-9]
INT             {DIGIT}+
DECIMAL         (([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))
FLOAT           {DECIMAL}
ALPHA           [a-zA-Z][a-zA-Z0-9_]*
ALNUM           [0-9a-zA-Z_]
BRANCO          [ ]
%option noyywrap

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"

extern void update_location(void);

extern int fileno(FILE *stream);


#define YY_NO_INPUT
#define YY_NO_UNPUT

#include "gen.calc.tokens.h"

%}

%%

"/*"[^*]*[*]+([^*/][^*]*[*]+)*[/]
"//"[^\n]*
"#"[^\n]*

"print"                         { return PRINT; }
{ALPHA}                         { yylval.s = my_strdup(yytext); return IDENTIFIER; }
{INT}                           { yylval.i = atoi(yytext); return NUMBERI; }
{FLOAT}                         { yylval.f = atof(yytext); return(NUMBERF); }
"("                             { return '('; }
")"                             { return ')'; }
"="                             { return '='; }
"-"                             { return '-'; }
"+"                             { return '+'; }
"*"                             { return '*'; }
"/"                             { return '/'; }
"^"                             { return '^'; }
"\n"                            { return '\n'; }

[ \t\r]+

%%


char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *t = malloc(len);
    if (t != NULL) {
        memcpy(t, s, len);
    }
    return t;
}

char *lex_filename = "";


int parseStream(char *name, FILE *stream) {
    lex_filename = name;
    yyset_in(stream);
    return yyparse();
}

const char *getParseStatusName(int status) {
    switch (status) {
    case 0:
        return "Success";
    case 1:
        return "Failure";
    default:
        return "Unknown";
    }
}




