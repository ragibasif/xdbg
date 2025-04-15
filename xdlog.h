// xdlog.h

#ifndef __XDLOG_H__
#define __XDLOG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdarg.h>
#include <stdio.h>

#ifdef XDLOG_PRINT
#define printf(...) dlog_printf(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define fprintf(s, ...)                                                        \
    dlog_fprintf(__FILE__, __LINE__, __func__, s, __VA_ARGS__)
#endif // XDLOG_PRINT

#define XDLOG_ANSI_RED "\x1b[0;91m"
#define XDLOG_ANSI_GREEN "\x1b[0;92m"
#define XDLOG_ANSI_YELLOW "\x1b[0;93m"
#define XDLOG_ANSI_BLUE "\x1b[0;94m"
#define XDLOG_ANSI_MAGENTA "\x1b[0;95m"
#define XDLOG_ANSI_CYAN "\x1b[0;96m"
#define XDLOG_ANSI_ITALIC "\x1b[3m"
#define XDLOG_ANSI_BOLD "\x1b[1m"
#define XDLOG_ANSI_RESET "\x1b[0m"

extern void dlog_printf(const char *file, unsigned int line,
                        const char *function, const char *format, ...);
extern void dlog_fprintf(const char *file, unsigned int line,
                         const char *function, FILE *stream, const char *format,
                         ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDLOG_H__
// xdlog.c
