%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"



%}

%start program
%token NUMBERI 
%token IDENTIFIER 
%token PRINT
%token NUMBERF
%token EOL

%%
    
program: 
    lines { printTree($1.node, 0, 'T');  executeTree($1.node); }
    ;

lines:
    statement { $$.node = $1.node; }
    | statement '\n' lines {$$.node = nodeLink($1.node, $3.node); }
    ;

statement:
        { $$.node = newNode(); }
    | expression 
        { $$.node = $1.node; }
    | assignment
        { $$.node = $1.node; }
    | PRINT expression 
        { $$.node = nodePrint($2.node); }
    ;

assignment:
    IDENTIFIER '=' expression
        { $$.node = nodeAssign(nodeVar($1.s), $3.node); }
    ;

expression:
    multiplication
        { $$.node = $1.node; }
    | multiplication '+' expression
        { $$.node = nodeMathOp(type_add, $1.node, $3.node); }
    | multiplication '-' expression
        { $$.node = nodeMathOp(type_sub, $1.node, $3.node); }
    ;

multiplication:
    power
        { $$.node = $1.node; }
    | power '*' power
        { $$.node = nodeMathOp(type_mul, $1.node, $3.node); }
    | power '/' power
        { $$.node = nodeMathOp(type_div, $1.node, $3.node); }
    ;

power:
    unary
        { $$.node = $1.node; }
    | unary '^' unary
        { $$.node = nodeMathOp(type_pow, $1.node, $3.node); }
    ;

unary:
    atom
        { $$.node = $1.node; }
    | '+' atom
        { $$.node = $2.node; }
    | '-' atom
        { $$.node = nodeNeg($2.node); }
    ;

atom:
    NUMBERF
        { $$.node = nodeFloat($1.f); }
    | NUMBERI
        { $$.node = nodeInt($1.i); }
    | IDENTIFIER
        { $$.node = nodeVar($1.s); }
    | '(' expression ')'
        { $$.node = $2.node; }
    ;

%%

#include <stdio.h>

struct identifier_node {
    char *name;
    int value;
    struct identifier_node *next;
};

struct identifier_node *identifiers_list = NULL;

void yyerror (){
  printf("Syntax error in input grammar.");
  exit(0);
}