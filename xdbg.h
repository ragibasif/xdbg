// xdbg.h

//
// TODO: Add flags to enable/disable verbosity
// TODO: Implement hash table to detect double frees.
// TODO: Add canary values before/after blocks to detect memory corruption
// TODO: Add documentation
// TODO: Add thread safety
// TODO: Add checks to match allocation and freeing of individual pointers by
// their address
// TODO: Add stack tracing (_maybe_)

#ifndef __XDBG_H__
#define __XDBG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(DEBUG) || defined(_DEBUG)
#define XDBG_ENABLE
#endif // defined(DEBUG) || defined(_DEBUG)

#ifndef XDBG_ENABLE
// #define XDBG_ENABLE
#endif // XDBG_ENABLE

#ifdef XDBG_ENABLE
// #define malloc(size) xdbg_malloc(size, __FILE__, __LINE__, __func__)
// #define realloc(size) xdbg_realloc(pointer,size, __FILE__, __LINE__,
// __func__) #define calloc(size) xdbg_calloc(number,size, __FILE__, __LINE__,
// __func__) #define free(pointer) xdbg_free(pointer, __FILE__, __LINE__,
// __func__)
// #define printf(...) xdbg_printf(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define XDBG_INITIALIZE() xdbg_initialize(__FILE__, __LINE__, __func__)
#define XDBG_REPORT() xdbg_report(__FILE__, __LINE__, __func__)
#define XDBG_FINALIZE() xdbg_finalize(__FILE__, __LINE__, __func__)
#else
#define XDBG_INITIALIZE()
#define XDBG_REPORT()
#define XDBG_FINALIZE()
#endif // XDBG_ENABLE

// ANSI escape codes for logging
#define XDBG_ANSI_RED "\x1b[0;91m"
#define XDBG_ANSI_GREEN "\x1b[0;92m"
#define XDBG_ANSI_YELLOW "\x1b[0;93m"
#define XDBG_ANSI_BLUE "\x1b[0;94m"
#define XDBG_ANSI_MAGENTA "\x1b[0;95m"
#define XDBG_ANSI_CYAN "\x1b[0;96m"
#define XDBG_ANSI_ITALIC "\x1b[3m"
#define XDBG_ANSI_BOLD "\x1b[1m"
#define XDBG_ANSI_RESET "\x1b[0m"

// Initializes the xdbg memory tracker and allocates resources. This is
// the first function that should be called.
extern void xdbg_initialize(const char *file, unsigned int line,
                            const char *function);

// Clears all memory tracking information and frees all allocated
// resources. This should be called at the very end.
extern void xdbg_finalize(const char *file, unsigned int line,
                          const char *function);

// Reports currently tracked allocations along with relevant information such as
// address, file, line, caller function, memory operations and size.
extern void xdbg_report(const char *file, unsigned int line,
                        const char *function);

// Allocates memory and tracks the allocation.
// Return pointer to the allocated memory, or NULL on failure.
extern void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                         const char *function);

// Allocates zero-initialized memory and tracks the allocation.
// Return pointer to the allocated memory, or NULL on failure.
extern void *xdbg_calloc(size_t number, size_t size, const char *file,
                         unsigned int line, const char *function);

// Reallocates memory and tracks the new allocation.
// Return pointer to the reallocated memory, or NULL on failure.
extern void *xdbg_realloc(void *pointer, size_t size, const char *file,
                          unsigned int line, const char *function);

// Wrapper around free() that tracks valid free operations and reports invalid
// free operations.
extern void xdbg_free(void *pointer, const char *file, unsigned int line,
                      const char *function);

// TODO: Implement
// Reports all memory allocations that were not freed.
// Prints detailed information about memory leaks.
extern void xdbg_report_leaks(void);

// TODO: Implement
// Resets all memory tracking state.
// Useful for test environments where tracking needs to be cleared between test
// cases.
extern void xdbg_reset_memory_tracker(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDBG_H__
