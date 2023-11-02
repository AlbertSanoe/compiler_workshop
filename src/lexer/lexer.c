#include <ctype.h> /* isspace, isdigit */
#include <stdbool.h>
#include <string.h>

#include "../lib/err.h"
#include "../lib/lexer.h"

// test
static const char *char_kind(TokenPtr ptr) {
  switch (ptr->kind) {
  case TK_PUNCT:
    return "TK_PUNCT";
  case TK_NUM:
    return "TK_NUM";
  case TK_EOF:
    return "TK_EOF";
  case TK_IDENT:
    return "TK_IDENT";
  }
}

// Current token
// Token *current_token;

// Input string
char *current_input;

// Consumes the current token if it matches `s`.
bool equal(const Token *tok, char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// Ensure that the current token is `s`.
Token *skip(Token *tok, char *s) {
  if (!equal(tok, s)) {
    error_tok(tok, "expected '%s'", s);
  }

  return tok->next;
}

// Create a new token.
static TokenPtr new_token(TokenKind kind, char *start, char *end) {
  TokenPtr tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = start;
  tok->len = end - start; // end is the first char that is not belong to
  return tok;
}

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Returns true if c is valid as the first character of an identifier.
static bool is_ident1(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// Returns true if c is valid as a non-first character of an identifier.
static bool is_ident2(char c) { return is_ident1(c) || ('0' <= c && c <= '9'); }

// Read a punctuator token from p and returns its length.
static int read_punct(char *p) {
  if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
      startswith(p, ">="))
    return 2;

  return ispunct(*p) ? 1 : 0;
}

// Tokenize `p` and returns new tokens.
TokenList tokenize(char *p) {
  current_input = p;
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
      continue;
    }

    // Identifier
    if (is_ident1(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident2(*p));
      cur = cur->next = new_token(TK_IDENT, start, p);
      continue;
    }

    // Punctuators
    int punct_len = read_punct(p);
    if (punct_len) {
      cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
      p += cur->len;
      continue;
    }

    error_at(p, "invalid token");
  }

  cur->next = new_token(TK_EOF, p, p);
  cur = cur->next;
  return head.next;
}