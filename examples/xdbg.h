#ifndef __XDBG_H__
#define __XDBG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*TODO: write function declarations*/
/*TODO: figure out how to do stack tracing */

/*****************************************************************************/
/*INCLUDES*/
/*****************************************************************************/

/*#include <execinfo.h> // for stack tracing*/
#include <ctype.h> /* size_t*/
#include <stdarg.h>
#include <stdbool.h> /* true, false, bool */
#include <stdio.h> /*Includes the standard I/O library for functions like `printf`.*/
#include <stdlib.h> /*Includes the standard library for functions like `malloc`, `free`, and `realloc`.*/
#include <string.h> /*Includes the string manipulation library for functions like `memcpy`.*/
#include <time.h> /*time*/

/*ANSI color codes for printing colored text.*/
/*Macros defining color codes.*/
#define XDBG_ANSI_RED "\x1b[91m"
#define XDBG_ANSI_GREEN "\x1b[92m"
#define XDBG_ANSI_YELLOW "\x1b[93m"
#define XDBG_ANSI_MAGENTA "\x1b[95m"
#define XDBG_ANSI_BLUE "\x1b[94m"
#define XDBG_ANSI_CYAN "\x1b[96m"
#define XDBG_ANSI_ITALIC "\x1b[3m"
#define XDBG_ANSI_UNDERLINE "\x1b[4m"
#define XDBG_ANSI_BOLD "\x1b[1m"
#define XDBG_ANSI_RESET "\x1b[0m"

/*****************************************************************************/
/*XDBG_ENABLE*/
/*****************************************************************************/

/*#ifndef XDBG_ENABLE*/
#ifdef XDBG_ENABLE
/*#define XDBG_ENABLE*/
#define malloc(size) xdbg_malloc(size, __FILE__, __LINE__, __func__)
#define free(pointer) xdbg_free(pointer, __FILE__, __LINE__, __func__)
#else
#define XDBG_LOG(msg)
#define XDBG_ERROR(msg)
#endif // XDBG_ENABLE

/*****************************************************************************/
/*DEBUG LOGGING*/
/*****************************************************************************/

/*XDBG API*/
extern void xdbg_init(void);
extern void xdbg_clear(void);
extern void xdbg_report(void);
extern void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                         const char *function);
extern void *xdbg_calloc(void);  /*TODO*/
extern void *xdbg_realloc(void); /*TODO*/
extern void xdbg_free(void *pointer, const char *file, unsigned int line,
                      const char *function);

/*****************************************************************************/
/*DEBUG MEMORY*/
/*****************************************************************************/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDBG_H__
