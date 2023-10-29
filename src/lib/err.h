#ifndef ERR_H
#define ERR_H

#include <stdarg.h> /* va_list, va_start, va_end */
#include <stdio.h> /* printf, fprintf, stderr */
#include <stdlib.h>

#include "lexer.h"

void error(char *fmt,...);

void verror_at(char *loc, char *fmt, va_list ap);

void error_at(char *loc, char *fmt, ...);

void error_tok(Token *tok, char *fmt, ...);

#endif