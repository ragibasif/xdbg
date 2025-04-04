#ifndef __XDBG_H__
#define __XDBG_H__

/* .·:''''''''''''''''''''''''''''''''''''':·. */
/* : :                                     : : */
/* : :  ██╗  ██╗██████╗ ██████╗  ██████╗   : : */
/* : :  ╚██╗██╔╝██╔══██╗██╔══██╗██╔════╝   : : */
/* : :   ╚███╔╝ ██║  ██║██████╔╝██║  ███╗  : : */
/* : :   ██╔██╗ ██║  ██║██╔══██╗██║   ██║  : : */
/* : :  ██╔╝ ██╗██████╔╝██████╔╝╚██████╔╝  : : */
/* : :  ╚═╝  ╚═╝╚═════╝ ╚═════╝  ╚═════╝   : : */
/* : :                                     : : */
/* '·:.....................................:·' */
/* '·:..................Ragib Asif.........:·' */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*****************************************************************************/
/*INCLUDES*/
/*****************************************************************************/

#include <ctype.h>   /* size_t*/
#include <stdbool.h> /* true, false, bool */
#include <stdio.h> /*Includes the standard I/O library for functions like `printf`.*/
#include <stdlib.h> /*Includes the standard library for functions like `malloc`, `free`, and `realloc`.*/
#include <string.h> /*memcpy*/

/*Macros defining ANSI color codes.*/
#define XDBG_ANSI_RED "\x1b[91m"
#define XDBG_ANSI_GREEN "\x1b[92m"
#define XDBG_ANSI_YELLOW "\x1b[93m"

#define XDBG_ANSI_MAGENTA "\x1b[95m"

#define XDBG_ANSI_BLUE "\x1b[94m"
#define XDBG_ANSI_CYAN "\x1b[96m"

#define XDBG_ANSI_ITALIC "\x1b[3m"
#define XDBG_ANSI_BOLD "\x1b[1m"

#define XDBG_ANSI_RESET "\x1b[0m"

/*****************************************************************************/
/*XDBG_ENABLE*/
/*****************************************************************************/

#ifndef XDBG_ENABLE
/*#define XDBG_ENABLE*/
#endif // XDBG_ENABLE

#ifdef XDBG_ENABLE
/*#define malloc(size) xdbg_malloc(size, __FILE__, __LINE__, __func__)*/
/*#define realloc(size) xdbg_realloc(pointer,size, __FILE__, __LINE__,
 * __func__)*/
/*#define calloc(size) xdbg_calloc(number,size, __FILE__, __LINE__, __func__)*/
/*#define free(pointer) xdbg_free(pointer, __FILE__, __LINE__, __func__)*/
#define XDBG_INIT() xdbg_init(__FILE__, __LINE__, __func__)
#define XDBG_REPORT() xdbg_report(__FILE__, __LINE__, __func__)
#define XDBG_CLEAR() xdbg_clear(__FILE__, __LINE__, __func__)
#else
#define XDBG_INIT()
#define XDBG_REPORT()
#define XDBG_CLEAR()
#endif // XDBG_ENABLE

/*****************************************************************************/
/*DEBUG MEMORY*/
/*****************************************************************************/

extern void xdbg_init(const char *file, unsigned int line,
                      const char *function);
extern void xdbg_clear(const char *file, unsigned int line,
                       const char *function);
extern void xdbg_report(const char *file, unsigned int line,
                        const char *function);
extern void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                         const char *function);
extern void *xdbg_calloc(size_t number, size_t size, const char *file,
                         unsigned int line, const char *function);
extern void *xdbg_realloc(void *pointer, size_t size, const char *file,
                          unsigned int line, const char *function);
extern void xdbg_free(void *pointer, const char *file, unsigned int line,
                      const char *function);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDBG_H__
