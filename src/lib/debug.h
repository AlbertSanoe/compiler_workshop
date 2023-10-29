#ifndef DRIVER_H
#define DRIVER_H

#include <stdarg.h> /* va_list, va_start, va_end */
#include <stdio.h>  /* printf, fprintf, stderr */

#define CURRENT_LINE __LINE__
#define CURRENT_FILE __FILE__
#define CURRENT_FUNC __func__

inline void info(const char *format, ...) {
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

#endif