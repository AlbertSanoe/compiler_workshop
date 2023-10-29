#include <assert.h>
#include "lib/debug.h"
#include "lib/err.h"
#include "lib/lexer.h"
#include "lib/parser.h"
#include "lib/genasm.h"

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
  int x;
  assert(depth == 0);
  return 0;
}