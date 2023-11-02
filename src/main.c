#include "lib/debug.h"
#include "lib/err.h"
#include "lib/genasm.h"
#include "lib/lexer.h"
#include "lib/parser.h"
#include <assert.h>

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of arguments", argv[0]);

  Token *tok = tokenize(argv[1]);
  Function *prog = parse(&tok);

  // Traverse the AST to emit assembly.
  codegen(prog);

  return 0;
}