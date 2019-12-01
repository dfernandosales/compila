#include "calc.h"

struct node *newNode() {
  struct node *nop = (struct node *)malloc(sizeof(struct node));
  nop->type = type_nop;
  return nop;
}

struct node *nodeInt(int value) {
  struct node *integer = (struct node *)malloc(sizeof(struct node));
  integer->type = type_int;
  integer->value_i = value;
  return integer;
}

struct node *nodeFloat(double value) {
  struct node *real = (struct node *)malloc(sizeof(struct node));
  real->type = type_float;
  real->value_f = value;
  return real;
}

struct node *nodeMathOp(enum type_op type, struct node *left,
                        struct node *right) {
  struct node *nodeMathOp = (struct node *)malloc(sizeof(struct node));
  nodeMathOp->right = right;
  nodeMathOp->left = left;
  nodeMathOp->op = type;
  nodeMathOp->type = type_mathop;
  return nodeMathOp;
}

struct node *nodeVar(char *name) {
  struct node *var = (struct node *)malloc(sizeof(struct node));
  strcpy(var->name, name);
  var->type = type_var;
  return var;
}

struct node *nodeNeg(struct node *no) {
  struct node *neg = (struct node *)malloc(sizeof(struct node));
  neg->left = no;
  neg->type = type_neg;
  return neg;
}

struct node *nodeAssign(struct node *var, struct node *value) {
  struct node *novo = (struct node *)malloc(sizeof(struct node));
  novo->type = type_assign;
  novo->left = var;
  novo->right = value;
  return novo;
}

struct node *nodePrint(struct node *exp) {
  struct node *print = (struct node *)malloc(sizeof(struct node));
  print->left = exp;
  print->type = type_print;
  return print;
}

struct node *nodeLink(struct node *expLeft, struct node *expRight) {
  struct node *link = (struct node *)malloc(sizeof(struct node));
  link->left = expLeft;
  link->right = expRight;
  link->type = type_link;
  return link;
}

void printTree(struct node *root, int nivel, char side) {
  if (root != NULL) {
    if (nivel) {
      for (size_t i = 0; i < nivel; i++) {
        printf(" ");
      }
    }
    switch (root->type) {
    case type_nop:
      printf("%c:nop\n", side);
      break;
    case type_int:
      printf("%c:int\n", side);
      break;
    case type_float:
      printf("%c:float\n", side);
      break;
    case type_var:
      printf("%c:var\n", side);
      break;
    case type_mathop:
      printf("%c:mathop\n", side);
      break;
    case type_neg:
      printf("%c:neg\n", side);
      break;
    case type_assign:
      printf("%c:=\n", side);
      break;
    case type_print:
      printf("%c:print\n", side);
      break;
    case type_link:
      printf("%c:link\n", side);
      break;

    default:
      break;
    }

    if (root->left != NULL)
      printTree(root->left, nivel + 4, 'L');

    if (root->right != NULL)
      printTree(root->right, nivel + 4, 'R');
  }
}

struct node *executeTree2(struct symbols *symbols, struct node *curr) {
  if (curr == NULL) {
    printf("Tree node is null.\n");
    return NULL;
  }

  switch (curr->type) {

  case type_link:
    executeTree2(symbols, curr->left);
    executeTree2(symbols, curr->right);
    break;

  case type_mathop: {
    struct node *left = executeTree2(symbols, curr->left);
    struct node *right = executeTree2(symbols, curr->right);
    struct node *result = newNode();

    if (left->type == right->type) {
      result->type = left->type;

      switch (curr->op) {
      case type_mul: {
        result->value_i = left->value_i * right->value_i;
        result->value_f = left->value_f * right->value_f;
        break;
      }

      case type_div: {
        result->value_i = left->value_i / right->value_i;
        result->value_f = left->value_f / right->value_f;
        break;
      }

      case type_add: {
        result->value_i = left->value_i + right->value_i;
        result->value_f = left->value_f + right->value_f;
        break;
      }

      case type_sub: {
        result->value_i = left->value_i - right->value_i;
        result->value_f = left->value_f - right->value_f;
        break;
      }

      case type_pow: {
        result->value_i =
            (int)powf((float)left->value_i, (float)right->value_i);
        result->value_f = (float)powf(left->value_f, right->value_f);
        break;
      }
      }
    } else {
      float lhs = left->type == type_int ? (float)left->value_i : left->value_f;
      float rhs =
          right->type == type_int ? (float)right->value_i : right->value_f;

      result->type = type_float;

      switch (curr->op) {
      case type_mul: {
        result->value_f = lhs * rhs;
        break;
      }

      case type_div: {
        result->value_f = lhs / rhs;
        break;
      }

      case type_add: {
        result->value_f = lhs + rhs;
        break;
      }

      case type_sub: {
        result->value_f = lhs - rhs;
        break;
      }

      case type_pow: {
        result->value_f = (float)pow(lhs, rhs);
        break;
      }
      }
    }

    return result;
  }

  case type_print: {
    struct node *resolved = executeTree2(symbols, curr->left);
    if (resolved->type == type_int)
      printf("PRINT %d\n", resolved->value_i);
    else if (resolved->type == type_float)
      printf("PRINT %.2f\n", resolved->value_f);
    else
      printf("PRINT ERROR\n");
    break;
  }

  case type_neg: {
    struct node *resolved = executeTree2(symbols, curr->left);
    if (resolved->type == type_int)
      resolved->value_i = -resolved->value_i;
    else if (resolved->type == type_float)
      resolved->value_f = -resolved->value_f;
    return resolved;
  }

  case type_int:
  case type_float:
    return curr;

  case type_var: {
    struct node *result = newNode();
    struct symbol *s = get_symbol(symbols, curr->name);
    result->type = s->type;
    result->value_i = s->value_i;
    result->value_f = s->value_f;
    return result;
  }

  case type_assign: {
    struct node *resolved = executeTree2(symbols, curr->right);
    if (contains_symbol(symbols, curr->left->name)) {
      struct symbol *s = get_symbol(symbols, curr->left->name);
      if (s->type == resolved->type) {
        s->value_i = resolved->value_i;
        s->value_f = resolved->value_f;
      } else {
        printf("ATTRIBUTION TYPE ERROR: Variable ->%s\n",curr->left->name);
        return NULL;
      }
    } else {
      struct symbol *s = insert_symbol(symbols, curr->left->name);
      strcpy(s->name, curr->left->name);
      s->type = resolved->type;
      s->value_i = resolved->value_i;
      s->value_f = resolved->value_f;
    }
    return NULL;
  }

  case type_nop:
    break;

  default:
    printf("UNINPLEMENTED %d\n", curr->type);
    break;
  }
}

void executeTree(struct node *root) {
  struct symbols *symbols = init();
  executeTree2(symbols, root);
}

struct symbols *init() {
  struct symbols *symbols = (struct symbols *)malloc(sizeof(struct symbols));
  symbols->length = 0;
  symbols->capacity = 2;
  symbols->list =
      (struct symbol **)malloc(sizeof(struct symbol *) * symbols->capacity);
  return symbols;
}

struct symbol *get_symbol(struct symbols *symbols, char *name) {
  for (int i = 0; i < symbols->length; i++) {
    struct symbol *curr = symbols->list[i];
    if (strcmp(curr->name, name) == 0) {
      return curr;
    }
  }
  printf("Symbol do not exist.");
  return NULL;
}

struct symbol *insert_symbol(struct symbols *symbols, char *name) {
  int curr = symbols->length++;
  if (symbols->length == symbols->capacity) {
    symbols->capacity *= 2;
    symbols->list = (struct symbol **)realloc(
        symbols->list, sizeof(struct symbol *) * symbols->capacity);
  }
  symbols->list[curr] = (struct symbol *)malloc(sizeof(struct symbol));
  return symbols->list[curr];
}

int contains_symbol(struct symbols *symbols, char *name) {
  for (int i = 0; i < symbols->length; i++) {
    struct symbol *curr = symbols->list[i];
    if (strcmp(curr->name, name) == 0) {
      return 1;
    }
  }
  return 0;
}