#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h> /* bool */

typedef enum {
  TK_IDENT, // Identifiers
  TK_PUNCT, // punctuators
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

// Current token
extern Token *current_token;

// Input program
extern char *current_input;

bool equal(const Token *tok, char *op);
Token *skip(Token *tok, char *s);
TokenList tokenize(char*input);

#endif