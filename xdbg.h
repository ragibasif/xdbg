// xdbg.h

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
/*#define XDBG_ENABLE*/
#endif // XDBG_ENABLE

/*****************************************************************************/
/*                                                                    MACROS */
/*****************************************************************************/

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
/*                                                                       API */
/*****************************************************************************/

/**
 * @brief Initializes the xdbg memory tracker.
 *
 * @param file The source file where this function is called (use __FILE__).
 * @param line The line number of the call site (use __LINE__).
 * @param function The function name of the caller (use __func__).
 */
extern void xdbg_init(const char *file, unsigned int line,
                      const char *function);

/**
 * @brief Clears all memory tracking information.
 *
 * @param file The source file where this function is called.
 * @param line The line number of the call site.
 * @param function The function name of the caller.
 */
extern void xdbg_clear(const char *file, unsigned int line,
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
 * @brief Validates the internal integrity of the memory tracking system.
 *
 * Checks consistency between internal data structures (e.g., linked list ↔ hash
 * map).
 */
extern void xdbg_check_integrity(void); // TODO: Implement

/**
 * @brief Resets all memory tracking state.
 *
 * Useful for test environments where tracking needs to be cleared between test
 * cases.
 */
extern void xdbg_reset_memory_tracker(void); // TODO: Implement

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDBG_H__
