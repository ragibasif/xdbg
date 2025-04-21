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

/* ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' */
/*                                                                           */
/*                       ██╗  ██╗██████╗ ██████╗  ██████╗                    */
/*                       ╚██╗██╔╝██╔══██╗██╔══██╗██╔════╝                    */
/*                        ╚███╔╝ ██║  ██║██████╔╝██║  ███╗                   */
/*                        ██╔██╗ ██║  ██║██╔══██╗██║   ██║                   */
/*                       ██╔╝ ██╗██████╔╝██████╔╝╚██████╔╝                   */
/*                       ╚═╝  ╚═╝╚═════╝ ╚═════╝  ╚═════╝                    */
/*                                                                           */
/* ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*****************************************************************************/
/*                                                                  INCLUDES */
/*****************************************************************************/

#include <ctype.h>   // size_t
#include <stdarg.h>  // va_list, va_start, va_end, __VA_ARGS__
#include <stdbool.h> // true, false, bool
#include <stdio.h> // Includes the standard I/O library for functions like `printf`
#include <stdlib.h> // Includes the standard library for functions like `malloc`, `free`, and `realloc`.
#include <string.h> // memcpy

/*****************************************************************************/
/*                                                                    ENABLE */
/*****************************************************************************/

#if defined(DEBUG) || defined(_DEBUG)
#define XDBG_ENABLE
#endif // defined(DEBUG) || defined(_DEBUG)

#ifndef XDBG_ENABLE
// #define XDBG_ENABLE
#endif // XDBG_ENABLE

/*****************************************************************************/
/*                                                                    MACROS */
/*****************************************************************************/

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

#define XDBG_ANSI_RED "\x1b[0;91m"
#define XDBG_ANSI_GREEN "\x1b[0;92m"
#define XDBG_ANSI_YELLOW "\x1b[0;93m"
#define XDBG_ANSI_BLUE "\x1b[0;94m"
#define XDBG_ANSI_MAGENTA "\x1b[0;95m"
#define XDBG_ANSI_CYAN "\x1b[0;96m"
#define XDBG_ANSI_ITALIC "\x1b[3m"
#define XDBG_ANSI_BOLD "\x1b[1m"
#define XDBG_ANSI_RESET "\x1b[0m"

/*****************************************************************************/
/*                                                                       API */
/*****************************************************************************/

/**
 * @brief Initializes the xdbg memory tracker.
 *
 * @param file The source file where this function is called (use __FILE__).
 * @param line The line number of the call site (use __LINE__).
 * @param function The function name of the caller (use __func__).
 */
extern void xdbg_initialize(const char *file, unsigned int line,
                            const char *function);

/**
 * @brief Clears all memory tracking information.
 *
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 */
extern void xdbg_finalize(const char *file, unsigned int line,
                          const char *function);

/**
 * @brief Reports currently tracked allocations.
 *
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 */
extern void xdbg_report(const char *file, unsigned int line,
                        const char *function);

/**
 * @brief Allocates memory and tracks the allocation.
 *
 * @param size The number of bytes to allocate.
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
extern void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                         const char *function);

/**
 * @brief Allocates zero-initialized memory and tracks the allocation.
 *
 * @param number Number of elements to allocate.
 * @param size Size of each element in bytes.
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
extern void *xdbg_calloc(size_t number, size_t size, const char *file,
                         unsigned int line, const char *function);

/**
 * @brief Reallocates memory and tracks the new allocation.
 *
 * @param pointer Pointer to the previously allocated memory.
 * @param size New size in bytes.
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 * @return Pointer to the reallocated memory, or NULL on failure.
 */
extern void *xdbg_realloc(void *pointer, size_t size, const char *file,
                          unsigned int line, const char *function);

/**
 * @brief Frees memory and removes it from the tracking system.
 *
 * @param pointer Pointer to the memory block to free.
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 */
extern void xdbg_free(void *pointer, const char *file, unsigned int line,
                      const char *function);

/**
 * @brief Reports all memory allocations that were not freed.
 *
 * Prints detailed information about memory leaks.
 */
extern void xdbg_report_leaks(void); // TODO: Implement

/**
 * @brief Resets all memory tracking state.
 *
 * Useful for test environments where tracking needs to be cleared between test
 * cases.
 */
extern void xdbg_reset_memory_tracker(void); // TODO: Implement

extern void xdbg_printf(const char *file, unsigned int line,
                        const char *function, const char *format, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDBG_H__
