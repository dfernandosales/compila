#ifndef CALC_H
#define CALC_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>

enum node_type {
  type_nop,
  type_int,
  type_float,
  type_var,
  type_mathop,
  type_neg,
  type_assign,
  type_print,
  type_link
};

enum type_op { type_add, type_sub, type_mul, type_div, type_pow };

struct node {
  enum node_type type;
  enum type_op op;
  struct node *right;
  struct node *left;
  int value_i;
  float value_f;
  char name[32];
  int errorLine;
};

struct symbol {
  char name[32];
  enum node_type type;
  int value_i;
  float value_f;
};

struct symbols {
  struct symbol **list;
  int length;
  int capacity;
};

union calc_value {
  int i;
  float f;
  char *s;
  int tipo;
  struct node *node;
};

struct node *newNode();

struct node *nodeInt(int value);

struct node *nodeFloat(double value);

struct node *nodeMathOp(enum type_op type, struct node *left,
                        struct node *right);

struct node *nodeVar(char *nome);

struct node *nodeNeg(struct node *filho);

struct node *nodeAssign(struct node *nome, struct node *valor);

struct node *nodePrint(struct node *expressao);

struct node *nodeLink(struct node *expEsq, struct node *expDir);

void printTree(struct node *root, int nivel, char side);

void executeTree(struct node *root);
struct node *executeTree2(struct symbols *symbols, struct node *curr);

struct node *rootAbstractTree;

struct symbols *init();
struct symbol *get_symbol(struct symbols *symbols, char *name);
struct symbol *insert_symbol(struct symbols *symbols, char *name);
int contains_symbol(struct symbols *symbols, char *name);

#define YYSTYPE union calc_value
extern YYSTYPE yylval;

extern char *lex_filename;
extern int lex_linenum;
extern int lex_colnum;

extern int parseStream(char *name, FILE *stream);
extern const char *getParseStatusName(int status);
extern char *my_strdup(const char *s);

extern void yyerror();
extern void yyset_in(FILE *_in_str);
extern int yylex(void);
extern int yyparse(void);
extern int yyget_lineno(void);

#endif /* CALC_H */
