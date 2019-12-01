
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

#define YY_USER_ACTION update_location();

#define YY_NO_INPUT
#define YY_NO_UNPUT

#include "gen.calc.tokens.h"

int lines= 0;
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
int lex_linenum = 1;
int lex_colnum = 0;

void update_location(void) {
    for (char *s = yytext; *s != '\0'; s++) {
        if (*s == '\n') { 
            lex_colnum = 0;
            lex_linenum++;
        }
        else if (*s == '\t') {
            lex_colnum += 8 - (lex_colnum % 8);
        }
        else {
            lex_colnum++;
        }
    }
#if 0
    ECHO;
#endif
}

int parse_int(int skipNumChars, int radix) {
    char *s = &(yytext[skipNumChars]);

    for (int i = 0; s[i] != '\0'; i++) {
        char c = s[i];

        if (c == '_' || c == 'u' || c == 'b' || c == 's' || c == 'i' || c == 'l') {
            char *t = my_strdup(s);

            do {
                if (c != '_') {
                    t[i] = '\0';
                    break;
                }

                int k = 0;

                for (int j = 0; s[j] != '\0'; j++) {
                    c = s[j];

                    if (c == 'u' || c == 'b' || c == 's' || c == 'i' || c == 'l') {
                        break;
                    }

                    if (s[j] != '_') {
                        t[k++] = s[j];
                    }
                }

                t[k] = '\0';
            }
            while (0);

            int retval = (int) strtol(t, NULL, radix);
            free(t);
            return retval;
        }
    }

    return (int) strtol(s, NULL, radix);
}

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




