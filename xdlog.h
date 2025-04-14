/**
 * @file xdlog.h
 * @author [Ragib Asif](https://github.com/ragibasif)
 */

#ifndef __XDLOG_H__
#define __XDLOG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdarg.h>

/**
 * @brief ANSI escape code macros for styling terminal output (color, bold,
 * italic).
 *
 * These macros can be used to print styled text to terminals that support ANSI
 * escape codes.
 */

#define XDLOG_ANSI_RED "\x1b[0;91m"
#define XDLOG_ANSI_GREEN "\x1b[0;92m"
#define XDLOG_ANSI_YELLOW "\x1b[0;93m"
#define XDLOG_ANSI_BLUE "\x1b[0;94m"
#define XDLOG_ANSI_MAGENTA "\x1b[0;95m"
#define XDLOG_ANSI_CYAN "\x1b[0;96m"
#define XDLOG_ANSI_ITALIC "\x1b[3m"
#define XDLOG_ANSI_BOLD "\x1b[1m"
#define XDLOG_ANSI_RESET "\x1b[0m"

extern void debugLog(const char *prefix, const char *color, const char *file, unsigned int line, const char *function, const char *format, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDLOG_H__

#ifdef XDLOG_IMPLEMENTATION

#include <stdio.h>

#define XDLOG(...) debugLog("LOG\0",XDLOG_ANSI_BLUE,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define XDLOG_ERROR(...) debugLog("ERROR\0",XDLOG_ANSI_RED,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define XDLOG_WARNING(...) debugLog("WARNING\0",XDLOG_ANSI_YELLOW,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define XDLOG_SUCCESS(...) debugLog("SUCCESS\0",XDLOG_ANSI_GREEN,__FILE__,__LINE__,__func__,__VA_ARGS__)

void debugLog(const char *prefix, const char *color, const char *file, unsigned int line, const char *function, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("(%s%s%s) ", XDLOG_ANSI_MAGENTA, "DLOG",
            XDLOG_ANSI_RESET);
    printf("%s%s%s %u %s%s ", XDLOG_ANSI_CYAN, XDLOG_ANSI_ITALIC, file, line,
            function, XDLOG_ANSI_RESET);
	printf("[%s%s%s%s] ", color,XDLOG_ANSI_BOLD,prefix,XDLOG_ANSI_RESET);
    vprintf(format, args); // Print the actual message
    va_end(args);
}

#endif // DLOG_IMPLEMENTATION
