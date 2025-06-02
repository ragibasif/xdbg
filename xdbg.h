// xdbg.h

#ifndef __XDBG_H__
#define __XDBG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "AEC.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef XDBG_ENABLE
#define malloc(size) xdbg_malloc(size, __FILE__, __LINE__, __func__)
#define realloc(pointer, size)                                                 \
    xdbg_realloc(pointer, size, __FILE__, __LINE__, __func__)
#define calloc(number, size)                                                   \
    xdbg_calloc(number, size, __FILE__, __LINE__, __func__)
#define free(pointer) xdbg_free(pointer, __FILE__, __LINE__, __func__)
#define XDBG_INITIALIZE() xdbg_initialize(__FILE__, __LINE__, __func__)
#define XDBG_REPORT() xdbg_report(__FILE__, __LINE__, __func__)
#define XDBG_FINALIZE() xdbg_finalize(__FILE__, __LINE__, __func__)
#else
#undef malloc
#undef realloc
#undef calloc
#undef free
#define XDBG_INITIALIZE()
#define XDBG_REPORT()
#define XDBG_FINALIZE()
#endif // XDBG_ENABLE

extern void xdbg_initialize(const char *file, unsigned int line,
                            const char *function);

extern void xdbg_finalize(const char *file, unsigned int line,
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
