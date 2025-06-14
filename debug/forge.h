//
/*Forge is a basic utility library, consisting of pure c functions that are
 * useful in any development. Forge is meant to be included in to all c files
 * and all other Quel Solaar libraries and applications. Because it is included
 * in all files it can provide some very useful functiuonality such as debugging
 * memory and help find memory leaks. Forge is designed to be lightweight and
 * completely platform and dependency independent.
 */

#ifndef __FORGE_H__
#define __FORGE_H__

#if !defined(F_NO_MEMORY_DEBUG)
// #define F_MEMORY_DEBUG /* turns on the memory debugging system */
// #define F_MEMORY_PRINT /* turns on the memory printing system */
#endif
#if !defined(F_EXIT_CRASH)
// #define F_EXIT_CRASH /* turns on the crash on exit */
#endif

/* ----- Debugging -----
If F_MEMORY_DEBUG  is enabled, the memory debugging system will create macros
that replace malloc, free and realloc and allows the system to kept track of
and report where memory is being allocated, how much and if the memory is
being freed. This is very useful for finding memory leaks in large
applications. The system can also over allocate memory and fill it with a magic
number and can therefore detect if the application writes outside of the
allocated memory. If F_EXIT_CRASH is defined, then exit(); will be replaced with
a function that writes to NULL. This will make it trivial ti find out where an
application exits using any debugger., */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern void f_debug_memory_init(
    void (*lock)(void *mutex), void (*unlock)(void *mutex),
    void *mutex); /* Required for memory debugger to be thread safe */
extern void *
f_debug_mem_malloc(size_t size, char *file,
                   unsigned int line); /* Replaces malloc and records the c file
                                  and line where it was called*/
extern void *
f_debug_mem_realloc(void *pointer, size_t size, char *file,
                    unsigned int line); /* Replaces realloc and records the c
                                   file and line where it was called*/
extern void
f_debug_mem_free(void *buf, char *file,
                 unsigned int line); /* Replaces free and records the c file
                                and line where it was called*/
extern bool
f_debug_mem_comment(void *buf,
                    char *comment); /* add a comment to an allocation that can
                                       help identyfy its use. */
extern void f_debug_mem_print(
    unsigned int
        min_allocs); /* Prints§ out a list of all allocations made, their
                location, how much memorey each has allocated, freed,
                and how many allocations have been made. The min_allocs
                parameter can be set to avoid printing any allocations
                that have been made fewer times then min_allocs */
extern void f_debug_mem_reset(
    void); /* f_debug_mem_reset allows you to clear all memory stored in the
              debugging system if you only want to record allocations after a
              specific point in your code*/
extern size_t
f_debug_mem_consumption(void); /* add up all memory consumed by mallocsd and
                                  reallocs covered by the memory debugger .*/
extern bool f_debug_mem_query(
    void *pointer, unsigned int *line, char **file,
    size_t *size); /* query the size and place of allocation of a pointer */
extern bool f_debug_mem_test(
    void *pointer, size_t size,
    bool ignore_not_found); /* query if a bit of memory is safe to access. */
extern bool
f_debug_memory(); /*f_debug_memory checks if any of the bounds of any allocation
                     has been over written and reports where to standard out.
                     The function returns TRUE if any error was found*/

extern void *f_debug_memory_fopen(const char *file_name, const char *mode,
                                  char *file, unsigned int line);
extern void f_debug_memory_fclose(void *f, char *file, unsigned int line);

#ifdef F_MEMORY_DEBUG

#define malloc(n)                                                              \
    f_debug_mem_malloc(n, __FILE__, __LINE__) /* Replaces malloc. */
#define realloc(n, m)                                                          \
    f_debug_mem_realloc(n, m, __FILE__, __LINE__)       /* Replaces realloc. */
#define free(n) f_debug_mem_free(n, __FILE__, __LINE__) /* Replaces free. */

#define fopen(n, m) f_debug_memory_fopen(n, m, __FILE__, __LINE__)
#define fclosee(n) f_debug_memory_fclose(n, __FILE__, __LINE__)

#else
#ifndef F_MEMORY_INTERNAL

#define f_debug_memory_init(n, m, k)
#define f_debug_mem_comment(n, m)
#define f_debug_mem_print(n)
#define f_debug_mem_reset()
#define f_debug_mem_consumption() 0
#define f_debug_mem_query(n, m, k, l)
#define f_debug_memory()

#endif
#endif

#ifdef F_EXIT_CRASH

extern void
exit_crash(unsigned int i); /* function guaranteed to crash (Writes to NULL).*/
#define exit(n)                                                                \
    exit_crash(                                                                \
        n) /* over writhing exit(0)  with a function guaranteed to crash.      \
            */

#endif

#endif // __FORGE_H__
