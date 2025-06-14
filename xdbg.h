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

/******************************************************************************/
/*                          DEBUGGING - DO NOT TOUCH                          */
/******************************************************************************/

#define F_MEMORY_DEBUG /* turns on the memory debugging system */

#ifdef F_MEMORY_DEBUG
#define F_MEMORY_PRINT /* turns on the memory printing system */
#define F_EXIT_CRASH   /* turns on the crash on exit */
#include "debug/dbg.h"
#include "debug/forge.h"
#endif

/******************************************************************************/

// #define xmalloc(size) x_malloc(size, __FILE__, __LINE__, __func__)
// #define xrealloc(pointer, size) \
//     x_realloc(pointer, size, __FILE__, __LINE__, __func__)
// #define xcalloc(number, size) \
//     x_calloc(number, size, __FILE__, __LINE__, __func__)
// #define xfree(pointer) x_free(pointer, __FILE__, __LINE__, __func__)

typedef enum Type { MALLOC, REALLOC, CALLOC, FREE } Type;

// TODO: implement
typedef struct Internal {
    void *pointer;
    size_t *size;
} Internal;

void internal_create(void);

void internal_destroy(void);

// TODO: implement
typedef struct Location {
    const char *file;
    const unsigned int *line;
    const char *function;
} Location;

void location_create(void);

void location_destroy(void);

// TODO: implement
typedef struct Freed {
    bool status;
    Location *location;
} Freed;

void freed_create(void);

void freed_destroy(void);

// TODO: implement
typedef struct Allocator {
    Internal *internal;
    Type type;
    Location *location;
    Freed *freed;
    struct Allocator *next;
    struct Allocator *previous;
} Allocator;

void allocator_create(void);

void allocator_destroy(void);

typedef struct Data {
    Allocator *head;
    Allocator *tail;
} Data;

void data_create(void);

void data_destroy(void);

typedef struct Report {
    int allocation_count;
    int free_count;
    size_t allocated_bytes;
    size_t freed_bytes;
} Report;

void report_create(void);

void report_destroy(void);

typedef struct Xdbg {
    Data *data;
    Report *report;
} Xdbg;

Xdbg *xdbg;

void x_create(void);

void x_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __XDBG_H__
