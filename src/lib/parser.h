#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

//
// Parser
//

typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_EXPR_STMT, // Expression statement
  ND_VAR,       // Variable
  ND_NUM,       // Integer
} NodeKind;

// AST node type
struct Node {
  NodeKind kind;    // Node kind
  struct Node *next; // Next Node
  struct Node *lhs; // Left-hand side
  struct Node *rhs; // Right-hand side
  char name;        // used if kind == ND_VAR
  int val;          // Used if kind == ND_NUM
};

typedef struct Node Node;
typedef struct Node *NodePtr;
typedef struct Node *NodeRootPtr;

Node *parse(Token **tok);

Node *get_expr(Token **tok);

Node *stmt(Token **tok);

Node *expr_stmt(Token **tok);

Node *assign(Token **tok);

Node *equality(Token **tok);

NodePtr relational(Token **tok);

Node *add(Token **tok);

Node *get_mul(Token **tok);

Node *get_unary(Token **tok);

Node *get_primary(Token **tok);

#endif