#include <stdlib.h>
#include <string.h>

#include "../lib/debug.h"
#include "../lib/err.h"
#include "../lib/lexer.h"
#include "../lib/parser.h"

void debug_node(NodePtr node_ptr) {
  if (node_ptr == NULL) {
    DEBUG("empty\n");
    return;
  }

  switch (node_ptr->kind) {

  case ND_ADD:
    printf("+");
    break;
  case ND_SUB:
    printf("-");
    break;
  case ND_MUL:
    printf("*");
    break;
  case ND_DIV:
    printf("/");
    break;
  case ND_NUM:
    printf("%d", node_ptr->val);
    break;
  case ND_NEG:
    printf("-");
    break;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    break;
  case ND_ASSIGN:
  case ND_EXPR_STMT:
  case ND_VAR:
    break;
  }
}

void debug_tree(NodeRootPtr root) {
  if (!root) {
    return;
  }
  debug_tree(root->lhs);
  debug_tree(root->rhs);
  debug_node(root);
  printf("\n");
}

// All local variable instances created during parsing are
// accumulated to this list.
Obj *locals;

// Find a local variable by name.
static Obj *find_var(Token *tok) {
  for (Obj *var = locals; var; var = var->next)
    if (strlen(var->name) == tok->len &&
        !strncmp(tok->loc, var->name, tok->len))
      return var;
  return NULL;
}

static NodePtr new_node(NodeKind kind) {
  NodePtr node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

static NodePtr new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  NodePtr node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

static NodePtr new_num(int val) {
  NodePtr node = new_node(ND_NUM);
  node->val = val;
  return node;
}

static Node *new_var_node(Obj *var) {
  Node *node = new_node(ND_VAR);
  node->var = var;
  return node;
}

static Obj *new_lvar(char *name) {
  Obj *var = calloc(1, sizeof(Obj));
  var->name = name;
  var->next = locals;
  locals = var;
  return var;
}

// program = stmt*
Function *parse(Token **tok) {
  Node head = {};
  Node *cur = &head;

  while ((*tok)->kind != TK_EOF)
    cur = cur->next = stmt(tok);

  Function *prog = calloc(1, sizeof(Function));
  prog->body = head.next;
  prog->locals = locals;
  return prog;
}

// stmt = expr-stmt
Node *stmt(Token **tok) { return expr_stmt(tok); }

// expr-stmt = expr ";"
Node *expr_stmt(Token **tok) {
  Node *node = new_unary(ND_EXPR_STMT, get_expr(tok));
  (*tok) = (*tok)->next;
  (*tok) = skip(*tok, ";");
  return node;
}

// expr = assign
Node *get_expr(Token **tok) { return assign(tok); }

// assign = equality ("=" assign)?
Node *assign(Token **tok) {
  Node *node = equality(tok);
  if (equal(*tok, "=")) {
    (*tok) = (*tok)->next;
    node = new_binary(ND_ASSIGN, node, assign(tok));
  }

  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(Token **tok) {
  Node *node = relational(tok);

  for (;;) {
    if (equal(*tok, "==")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_EQ, node, relational(tok));
      continue;
    }

    if (equal(*tok, "!=")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_NE, node, relational(tok));
      continue;
    }

    return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
NodePtr relational(Token **tok) {
  Node *node = add(tok);

  for (;;) {
    if (equal(*tok, "<")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_LT, node, add(tok));
      continue;
    }

    if (equal(*tok, "<=")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_LE, node, add(tok));
      continue;
    }

    if (equal(*tok, ">")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_LT, add(tok), node);
      continue;
    }

    if (equal(*tok, ">=")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_LE, add(tok), node);
      continue;
    }
    return node;
  }
}

// expr = mul ("+" mul | "-" mul)*
Node *add(Token **tok) {
  Node *node = get_mul(tok);
  DEBUG("%s\n", char_kind(*tok));
  for (;;) {
    if (equal(*tok, "+")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_ADD, node, get_mul(tok));
      continue;
    }
    if (equal(*tok, "-")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_SUB, node, get_mul(tok));
      continue;
    }
    return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
Node *get_mul(Token **tok) {
  Node *node = get_unary(tok);
  DEBUG("%s\n", char_kind(*tok));
  for (;;) {
    if (equal(*tok, "*")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_MUL, node, get_unary(tok));
      continue;
    }

    if (equal(*tok, "/")) {
      (*tok) = (*tok)->next;
      node = new_binary(ND_DIV, node, get_unary(tok));
      continue;
    }
    DEBUG("no * and / \n");
    return node;
  }
}

// unary = ("+" | "-")? unary
//       | primary
Node *get_unary(Token **tok) {
  if (equal(*tok, "+")) {
    (*tok) = (*tok)->next;
    return get_unary(tok);
  }

  if (equal(*tok, "-")) {
    (*tok) = (*tok)->next;
    return new_unary(ND_NEG, get_unary(tok));
  }

  return get_primary(tok);
}

// primary = "(" expr ")" | num | ident
Node *get_primary(Token **tok) {
  if (equal(*tok, "(")) {
    *tok = (*tok)->next;
    Node *node = get_expr(tok);
    *tok = skip(*tok, ")");
    return node;
  }

  if ((*tok)->kind == TK_NUM) {
    DEBUG("%s\n", char_kind(*tok));
    DEBUG("%d\n", (*tok)->val);
    Node *node = new_num((*tok)->val);
    *tok = (*tok)->next;
    DEBUG("%s\n", char_kind(*tok));
    return node;
  }

  if ((*tok)->kind == TK_IDENT) {
    Obj *var = find_var(*tok);
    if (!var) {
      char *val_dup = malloc((*tok)->len);
      // Copy the string
      val_dup = strncpy(val_dup, (*tok)->loc, (*tok)->len);
      var = new_lvar(val_dup);
    }

    *tok = (*tok)->next;
    return new_var_node(var);
  }

  error_tok(*tok, "expected an expression");
  return NULL;
}