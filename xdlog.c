#include "xdlog.h"

#undef printf
#undef fprintf

static void dlog_location(const char *file, unsigned int line,
                          const char *function) {
    printf("[%s%sfile: %s line: %u function: %s%s] ", XDLOG_ANSI_CYAN,
           XDLOG_ANSI_ITALIC, file, line, function, XDLOG_ANSI_RESET);
}

void dlog_printf(const char *file, unsigned int line, const char *function,
                 const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("(%s%s%s%s) ", XDLOG_ANSI_MAGENTA, XDLOG_ANSI_BOLD, "printf",
           XDLOG_ANSI_RESET);
    dlog_location(file, line, function);
    vprintf(format, args); // Print the actual message
    va_end(args);
}

void dlog_fprintf(const char *file, unsigned int line, const char *function,
                  FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("(%s%s%s%s) ", XDLOG_ANSI_MAGENTA, XDLOG_ANSI_BOLD, "fprintf",
           XDLOG_ANSI_RESET);
    dlog_location(file, line, function);
    vfprintf(stream, format, args); // Print the actual message
    va_end(args);
}
