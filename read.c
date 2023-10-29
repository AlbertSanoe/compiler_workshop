#include <assert.h>
#include <ctype.h>   /* isspace, isdigit */
#include <stdarg.h>  /* va_list, va_start, va_end */
#include <stdbool.h> /* bool */
#include <stdio.h>   /* printf, fprintf, stderr */
#include <stdlib.h>  /* calloc, strtol, exit */
#include <string.h>

#define CURRENT_LINE __LINE__
#define CURRENT_FILE __FILE__
#define CURRENT_FUNC __func__

void info(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);
}

// #define DEBUG_SIGN

#ifdef DEBUG_SIGN
#define DEBUG(...)                                                             \
  do {                                                                         \
    char buffer[100];                                                          \
    int n = sprintf(buffer, "[DEBUG] (%s, %d)", CURRENT_FILE, CURRENT_LINE);   \
    info("%-50s\t", buffer);                                                   \
    info(__VA_ARGS__);                                                         \
  } while (0)
#else
#define DEBUG(...)
#endif

#define INFO(...)                                                              \
  do {                                                                         \
    char buffer[100];                                                          \
    int n = sprintf(buffer, "[INFO] (%s, %d)", CURRENT_FILE, CURRENT_LINE);    \
    info("%-50s\t", buffer);                                                   \
    info(__VA_ARGS__);                                                         \
  } while (0)

typedef enum {
  TK_PUNCT, // Keywords or punctuators
  TK_NUM,   // Integer literals
  TK_EOF,   // End-of-file markers
} TokenKind;

// Token type
struct Token {
  TokenKind kind;     // Token kind
  struct Token *next; // Next token
  int val;            // If kind is TK_NUM, its value
  char *loc;          // Token location
  int len;
};

typedef struct Token Token;
typedef struct Token *TokenPtr;
typedef struct Token *TokenList;

const char *char_kind(TokenPtr ptr) {
  switch (ptr->kind) {

  case TK_PUNCT:
    return "TK_PUNCT";
  case TK_NUM:
    return "TK_NUM";
  case TK_EOF:
    return "TK_EOF";
  }
}

// Current token
Token *current_token;

// Input program
static char *current_input;

// Reports an error and exit.
static void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an error location and exit.
static void verror_at(char *loc, char *fmt, va_list ap) {
  int pos = loc - current_input;
  fprintf(stderr, "%s\n", current_input);
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

static void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(loc, fmt, ap);
}

static void error_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->loc, fmt, ap);
}

// Consumes the current token if it matches `s`.
static bool equal(const Token *tok, char *op) {
  // int i;
  // char *c;
  // for (c = tok->loc, i = 0; i < tok->len; c++, i++) {
  //   printf("%c", *c);
  // }
  // printf("\t%d\n", __LINE__);
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// Ensure that the current token is `s`.
static Token *skip(Token *tok, char *s) {
  if (!equal(tok, s)) {
    error_tok(tok, "expected '%s'", s);
  }

  return tok->next;
}

// Ensure that the current token is TK_NUM.
static int get_number(Token *tok) {
  if (tok->kind != TK_NUM)
    error_tok(tok, "expected a number");
  return tok->val;
}

// Create a new token.
static TokenPtr new_token(TokenKind kind, char *start, char *end) {
  TokenPtr tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = start;
  tok->len = end - start; // end is the first char that is not belong to
  return tok;
}

// Tokenize `p` and returns new tokens.
TokenList tokenize() {
  char *p = current_input;
  Token head = {};
  head.next = NULL;
  TokenPtr cur = &head;

  while (*p) {
    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Numeric literal
    if (isdigit(*p)) {
      cur->next = new_token(TK_NUM, p, p);
      cur = cur->next;
      char *q = p;
      cur->val = strtoul(p, &p, 10);
      cur->len = p - q;
      // printf("%s %d: the current len is %d\n", __FILE__, __LINE__, cur->len);
      continue;
    }

    // Punctuators
    if (ispunct(*p)) {
      cur = cur->next = new_token(TK_PUNCT, p, p + 1);
      p++;
      continue;
    }

    error_at(p, "invalid token");
  }

  cur->next = new_token(TK_EOF, p, p);
  cur = cur->next;
  return head.next;
}

//
// Parser
//

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NEG, // unary -
  ND_NUM, // Integer
} NodeKind;

// AST node type
struct Node {
  NodeKind kind;    // Node kind
  struct Node *lhs; // Left-hand side
  struct Node *rhs; // Right-hand side
  int val;          // Used if kind == ND_NUM
};

typedef struct Node Node;
typedef struct Node *NodePtr;
typedef struct Node *NodeRootPtr;

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

static NodePtr get_expr(Token **rest);
static NodePtr get_mul(Token **rest);
static NodePtr get_unary(Token **rest);
static NodePtr get_primary(Token **rest);

// static NodePtr get_expr(Token **rest, Token *tok);
// static NodePtr get_mul(Token **rest, Token *tok);
// static NodePtr get_primary(Token **rest, Token *tok);

// // expr = mul ("+" mul | "-" mul)*
// static Node *get_expr(TokenList *rest, Token *tok) {
//   Node *node;
//   node = get_mul(&tok, tok);

//   for (;;) {
//     if (equal(tok, "+")) {
//       node = new_binary(ND_ADD, node, get_mul(&tok, tok->next));
//       continue;
//     }

//     if (equal(tok, "-")) {
//       node = new_binary(ND_SUB, node, get_mul(&tok, tok->next));
//       continue;
//     }

//     *rest = tok;
//     return node;
//   }
// }

// // mul = primary ("*" primary | "/" primary)*
// static Node *get_mul(Token **rest, Token *tok) {
//   Node *node = get_primary(&tok, tok);

//   for (;;) {
//     if (equal(tok, "*")) {
//       node = new_binary(ND_MUL, node, get_primary(&tok, tok->next));
//       continue;
//     }

//     if (equal(tok, "/")) {
//       node = new_binary(ND_DIV, node, get_primary(&tok, tok->next));
//       continue;
//     }

//     *rest = tok;
//     return node;
//   }
// }

// // primary = "(" expr ")" | num
// static Node *get_primary(TokenList *rest, Token *tok) {
//   if (equal(tok, "(")) {
//     Node *node = get_expr(&tok, tok->next);
//     *rest = skip(tok, ")");
//     return node;
//   }

//   if (tok->kind == TK_NUM) {
//     Node *node = new_num(tok->val);
//     *rest = tok->next;
//     return node;
//   }

//   error_tok(tok, "expected an expression");
//   return NULL;
// }

// expr = mul ("+" mul | "-" mul)*
static Node *get_expr(Token **tok) {
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
static Node *get_mul(Token **tok) {
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
static Node* get_unary(Token **tok){
  if(equal(*tok,"+")){
    (*tok)=(*tok)->next;
    return get_unary(tok);
  }

  if (equal(*tok,"-")){
    (*tok)=(*tok)->next;
    return new_unary(ND_NEG,get_unary(tok));
  }

  return get_primary(tok);
}

// primary = "(" expr ")" | num
static Node *get_primary(Token **tok) {
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

  error_tok(*tok, "expected an expression");
  return NULL;
}

//
// Code generator
//

static int depth;

static void push(void) {
  printf("  push %%rax\n");
  depth++;
}

static void pop(const char *arg) {
  printf("  pop %s\n", arg);
  depth--;
}

static void gen_expr(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  mov $%d, %%rax\n", node->val);
    return;
  case ND_NEG:
    gen_expr(node->lhs);
    printf("  neg %%rax\n");
    return;
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
    break;
  }

  gen_expr(node->rhs);
  push();
  gen_expr(node->lhs);
  pop("%rdi");

  switch (node->kind) {
  case ND_ADD:
    printf("  add %%rdi, %%rax\n");
    return;
  case ND_SUB:
    printf("  sub %%rdi, %%rax\n");
    return;
  case ND_MUL:
    printf("  imul %%rdi, %%rax\n");
    return;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv %%rdi\n");
    return;
  case ND_NEG:
  case ND_NUM:
    return;
  }

  error("invalid expression");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s: invalid number of arguments", argv[0]);
  }
  // DEBUG("Hello World\n");

  // Tokenize and parse.
  current_input = argv[1];
  Token *tok = tokenize();
  Node *node = get_expr(&tok);
  // debug_tree(node);
  if (tok->kind != TK_EOF)
    error_tok(tok, "extra token");

  printf("  .globl main\n");
  printf("main:\n");

  // Traverse the AST to emit assembly.
  gen_expr(node);
  printf("  retq\n");

  assert(depth == 0);
  return 0;
}