#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define F_MEMORY_INTERNAL
#define F_NO_MEMORY_DEBUG
#define F_MEMORY_PRINT
#include "forge.h"

// Reset all attributes
#define RESET "\x1b[0m"

// Text colors
#define RED "\x1b[91m"
#define BLUE "\x1b[94m"

// Text attributes
#define BOLD "\x1b[1m"
#define DIM "\x1b[2m"

// Reset specific attributes
#define NO_BOLD "\x1b[21m" // or "\x1b[22m" (both work)
#define NO_ITALIC "\x1b[23m"
#define NO_UNDERLINE "\x1b[24m"
#define NO_BLINK "\x1b[25m"
#define NO_REVERSE "\x1b[27m"
#define NO_HIDDEN "\x1b[28m"
#define NO_STRIKETHROUGH "\x1b[29m"

// Cursor movement
#define CURSOR_UP(n) "\x1b[" #n "A"
#define CURSOR_DOWN(n) "\x1b[" #n "B"
#define CURSOR_FORWARD(n) "\x1b[" #n "C"
#define CURSOR_BACK(n) "\x1b[" #n "D"
#define CURSOR_NEXT_LINE(n) "\x1b[" #n "E"
#define CURSOR_PREV_LINE(n) "\x1b[" #n "F"
#define CURSOR_COLUMN(n) "\x1b[" #n "G"
#define CURSOR_POSITION(row, col) "\x1b[" #row ";" #col "H"

// Save and restore cursor position
#define CURSOR_SAVE "\x1b[s"
#define CURSOR_RESTORE "\x1b[u"

#define F_MEMORY_OVER_ALLOC 256
#define F_MEMORY_MAGIC_NUMBER 132
typedef struct {
    unsigned int size;
    void *buf;
    char *comment;
} STMemAllocBuf;

typedef struct {
    unsigned int line;
    char file[256];
    STMemAllocBuf *allocs;
    unsigned int alloc_count;
    size_t alloc_allocated;
    size_t size;
    size_t allocated;
    size_t freed;
} STMemAllocLine;

STMemAllocLine f_alloc_lines[1024];
unsigned int f_alloc_line_count = 0;

#define FORGE_FREE_POINTER_BUFFER_SIZE 1024

typedef struct {
    unsigned int alloc_line;
    char alloc_file[256];
    unsigned int free_line;
    char free_file[256];
    size_t size;
    void *pointer;
    bool realloc;
} STMemFreeBuf;

STMemFreeBuf f_freed_memory[FORGE_FREE_POINTER_BUFFER_SIZE];
unsigned int f_freed_memory_current = 0;
unsigned int f_freed_memory_count = 0;

void *f_alloc_mutex = NULL;
void (*f_alloc_mutex_lock)(void *mutex) = NULL;
void (*f_alloc_mutex_unlock)(void *mutex) = NULL;

void f_debug_memory_init(void (*lock)(void *mutex), void (*unlock)(void *mutex),
                         void *mutex) {
    f_alloc_mutex = mutex;
    f_alloc_mutex_lock = lock;
    f_alloc_mutex_unlock = unlock;
}

void *f_debug_memory_fopen(const char *file_name, const char *mode, char *file,
                           unsigned int line) {
    FILE *f;
    f = fopen(file_name, mode);
    return f;
}

void f_debug_memory_fclose(void *f, char *file, unsigned int line) {
    fclose(f);
}

bool f_debug_memory() {
    bool output = false;
    unsigned int i, j, k;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++) {
        for (j = 0; j < f_alloc_lines[i].alloc_count; j++) {
            unsigned char *buf;
            size_t size;
            buf = f_alloc_lines[i].allocs[j].buf;
            size = f_alloc_lines[i].allocs[j].size;
            for (k = 0; k < F_MEMORY_OVER_ALLOC; k++)
                if (buf[size + k] != F_MEMORY_MAGIC_NUMBER)
                    break;
            if (k < F_MEMORY_OVER_ALLOC) {
                if (f_alloc_lines[i].allocs[j].comment == NULL) {
                    printf(
                        DIM BOLD RED
                        "MEMORY ERROR: Overshoot at line %u in file %s\n" RESET,
                        f_alloc_lines[i].line, f_alloc_lines[i].file);

                    printf(RESET);
                } else {
                    printf(DIM BOLD RED
                           "MEMORY ERROR: Overshoot at line %u in file %s /* "
                           "%s */\n" RESET,
                           f_alloc_lines[i].line, f_alloc_lines[i].file,
                           f_alloc_lines[i].allocs[j].comment);

                    printf(RESET);
                }
                {
                    unsigned int *X = NULL;
                    X[0] = 0;
                }
                output = true;
            }
        }
        //		if(output)
        //			exit(0);
    }
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return output;
}

void f_debug_mem_print(unsigned int min_allocs) {
    unsigned int i, j;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    printf(DIM
           "Memory "
           "report:\n----------------------------------------------\n" RESET);

    printf(RESET);
    for (i = 0; i < f_alloc_line_count; i++) {
        if (min_allocs < f_alloc_lines[i].allocated - f_alloc_lines[i].freed) {
            printf(DIM "%s line: %u\n" RESET, f_alloc_lines[i].file,
                   f_alloc_lines[i].line);

            printf(RESET);
            printf(DIM
                   " - Bytes allocated: %zu\n - Allocations: %zu\n - Frees: "
                   "%zu\n\n" RESET,
                   f_alloc_lines[i].size, f_alloc_lines[i].allocated,
                   f_alloc_lines[i].freed);

            printf(RESET);
            for (j = 0; j < f_alloc_lines[i].alloc_count; j++)
                if (f_alloc_lines[i].allocs[j].comment != NULL)
                    printf(DIM "\t\t comment %p : %s\n" RESET,
                           f_alloc_lines[i].allocs[j].buf,
                           f_alloc_lines[i].allocs[j].comment);

            printf(RESET);
        }
    }
    printf(DIM "----------------------------------------------\n" RESET);

    printf(RESET);
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
}

void f_debug_mem_add(void *pointer, size_t size, char *file,
                     unsigned int line) {
    unsigned int i, j;
    for (i = 0; i < F_MEMORY_OVER_ALLOC; i++)
        ((unsigned char *)pointer)[size + i] = F_MEMORY_MAGIC_NUMBER;

    for (i = 0; i < f_alloc_line_count; i++) {
        if (line == f_alloc_lines[i].line) {
            for (j = 0; file[j] != 0 && file[j] == f_alloc_lines[i].file[j];
                 j++)
                ;
            if (file[j] == f_alloc_lines[i].file[j])
                break;
        }
    }
    if (i < f_alloc_line_count) {
        if (f_alloc_lines[i].alloc_allocated == f_alloc_lines[i].alloc_count) {
            f_alloc_lines[i].alloc_allocated += 1024;
            f_alloc_lines[i].allocs = realloc(
                f_alloc_lines[i].allocs, (sizeof *f_alloc_lines[i].allocs) *
                                             f_alloc_lines[i].alloc_allocated);
            if (f_alloc_lines[i].allocs == NULL) {
                printf(DIM BOLD RED
                       "MEMORY ERROR: Realloc returns NULL when trying to "
                       "allocate %zu "
                       "bytes at line %u in file %s\n" RESET,
                       size, line, file);

                printf(RESET);
                exit(0);
            }
        }
        f_alloc_lines[i].allocs[f_alloc_lines[i].alloc_count].size = size;
        f_alloc_lines[i].allocs[f_alloc_lines[i].alloc_count].comment = NULL;
        f_alloc_lines[i].allocs[f_alloc_lines[i].alloc_count++].buf = pointer;
        f_alloc_lines[i].size += size;
        f_alloc_lines[i].allocated++;
    } else {
        if (i < 1024) {
            f_alloc_lines[i].line = line;
            for (j = 0; j < 255 && file[j] != 0; j++)
                f_alloc_lines[i].file[j] = file[j];
            f_alloc_lines[i].file[j] = 0;
            f_alloc_lines[i].alloc_allocated = 256;
            f_alloc_lines[i].allocs = malloc((sizeof *f_alloc_lines[i].allocs) *
                                             f_alloc_lines[i].alloc_allocated);
            f_alloc_lines[i].allocs[0].size = size;
            f_alloc_lines[i].allocs[0].buf = pointer;
            f_alloc_lines[i].allocs[0].comment = NULL;
            f_alloc_lines[i].alloc_count = 1;
            f_alloc_lines[i].size = size;
            f_alloc_lines[i].freed = 0;
            f_alloc_lines[i].allocated++;
            f_alloc_line_count++;
        }
    }
    for (i = 0; i < f_freed_memory_count; i++) {
        if (pointer == f_freed_memory[i].pointer) {
            f_freed_memory[i] = f_freed_memory[--f_freed_memory_count];
            break;
        }
    }
}

void *f_debug_mem_malloc(size_t size, char *file, unsigned int line) {
    void *pointer;
    unsigned int i;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    f_debug_memory();
    pointer = malloc(size + F_MEMORY_OVER_ALLOC);

#ifdef F_MEMORY_PRINT
    printf(DIM BLUE "Malloc %zu bytes at pointer %p at %s line %u\n" RESET,
           size, pointer, file, line);
    printf(RESET);

#endif
    if (pointer == NULL) {
        printf(DIM BOLD RED
               "MEMORY ERROR: Malloc returns NULL when trying to allocate %zu "
               "bytes at "
               "line %u in file %s\n" RESET,
               size, line, file);

        printf(RESET);
        if (f_alloc_mutex != NULL)
            f_alloc_mutex_unlock(f_alloc_mutex);
        f_debug_mem_print(0);
        exit(0);
    }
    for (i = 0; i < size + F_MEMORY_OVER_ALLOC; i++)
        ((unsigned char *)pointer)[i] = F_MEMORY_MAGIC_NUMBER + 1;
    f_debug_mem_add(pointer, size, file, line);

    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return pointer;
}

bool f_debug_mem_remove(void *buf, char *file, unsigned int line, bool realloc,
                        size_t *size) {
    STMemFreeBuf *f;
    unsigned int i, j, k;

    f = &f_freed_memory[f_freed_memory_current];
    f_freed_memory_current =
        (f_freed_memory_current + 1) % FORGE_FREE_POINTER_BUFFER_SIZE;
    if (f_freed_memory_current > f_freed_memory_count)
        f_freed_memory_count = f_freed_memory_current;
    for (i = 0; i < 255 && file[i] != 0; i++)
        f->free_file[i] = file[i];
    f->free_file[i] = 0;
    f->free_line = line;
    f->realloc = realloc;
    f->pointer = buf;

    for (i = 0; i < f_alloc_line_count; i++) {
        for (j = 0; j < f_alloc_lines[i].alloc_count; j++) {
            if (f_alloc_lines[i].allocs[j].buf == buf) {
                for (k = 0; k < F_MEMORY_OVER_ALLOC; k++)
                    if (((unsigned char *)
                             buf)[f_alloc_lines[i].allocs[j].size + k] !=
                        F_MEMORY_MAGIC_NUMBER)
                        break;
                if (k < F_MEMORY_OVER_ALLOC) {
                    unsigned int *a = NULL;
                    printf(
                        DIM BOLD RED
                        "MEMORY ERROR: Overshoot at line %u in file %s\n" RESET,
                        f_alloc_lines[i].line, f_alloc_lines[i].file);

                    printf(RESET);
                    exit(0);
                    a[0] = 0;
                }
                for (k = 0; k < f_alloc_lines[i].allocs[j].size; k++)
                    ((unsigned char *)buf)[k] = 255;
                f->alloc_line = f_alloc_lines[i].line;
                for (k = 0; k < 255 && f_alloc_lines[i].file[k] != 0; k++)
                    f->alloc_file[k] = f_alloc_lines[i].file[k];
                f->alloc_file[k] = 0;
                f->size = f_alloc_lines[i].allocs[j].size;
                *size = f_alloc_lines[i].allocs[j].size;
                f_alloc_lines[i].size -= f_alloc_lines[i].allocs[j].size;
                f_alloc_lines[i].allocs[j] =
                    f_alloc_lines[i].allocs[--f_alloc_lines[i].alloc_count];
                f_alloc_lines[i].freed++;
                return true;
            }
        }
    }
    for (i = 0; i < f_freed_memory_count; i++) {
        if (f != &f_freed_memory[i] && buf == f_freed_memory[i].pointer) {
            unsigned int *a = NULL;
            if (f->realloc) {
                printf(DIM BOLD RED
                       "MEMORY ERROR: Pointer %p in file is freed twice! It "
                       "was freed on "
                       "line %u in %s, was reallocated to %zu bytes long one "
                       "line %u in "
                       "file %s\n" RESET,
                       f->pointer, f->free_line, f->free_file, f->size,
                       f->alloc_line, f->alloc_file);

                printf(RESET);
            } else {
                printf(DIM BOLD RED
                       "MEMORY ERROR: Pointer %p in file is freed twice! It "
                       "was freed on "
                       "line %u in %s, was allocated to %zu bytes long one "
                       "line %u in "
                       "file %s\n" RESET,
                       f->pointer, f->free_line, f->free_file, f->size,
                       f->alloc_line, f->alloc_file);
                printf(RESET);
            }
            return false;
        }
    }
    return true;
}

void f_debug_mem_free(void *buf, char *file, unsigned int line) {
    STMemFreeBuf b;
    size_t size = 0;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    if (!f_debug_mem_remove(buf, file, line, false, &size)) {
        unsigned int *X = NULL;
        X[0] = 0;
    }

#ifdef F_MEMORY_PRINT
    printf(DIM BLUE "Free %zu bytes at pointer %p at %s line %u\n" RESET, size,
           buf, file, line);

    printf(RESET);

#endif

    free(buf);
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
}

bool f_debug_mem_comment(void *buf, char *comment) {
    unsigned int i, j, k;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++) {
        for (j = 0; j < f_alloc_lines[i].alloc_count; j++) {
            if (f_alloc_lines[i].allocs[j].buf == buf) {
                f_alloc_lines[i].allocs[j].comment = comment;
                return true;
            }
        }
    }
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return false;
}

void *f_debug_mem_realloc(void *pointer, size_t size, char *file,
                          unsigned int line) {
    size_t i, j = 0, k, move;
    void *pointer2;

    if (pointer == NULL)
        return f_debug_mem_malloc(size, file, line);

    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++) {
        for (j = 0; j < f_alloc_lines[i].alloc_count; j++)
            if (f_alloc_lines[i].allocs[j].buf == pointer)
                break;
        if (j < f_alloc_lines[i].alloc_count)
            break;
    }
    if (i == f_alloc_line_count) {
        printf(DIM BOLD RED
               "FORGE MEMORY debugger error. Trying to reallocate pointer %p "
               "in %s "
               "line %u. Pointer is not allocated\n" RESET,
               pointer, file, line);

        printf(RESET);
        for (i = 0; i < f_alloc_line_count; i++) {
            for (j = 0; j < f_alloc_lines[i].alloc_count; j++) {
                unsigned int *buf;
                buf = f_alloc_lines[i].allocs[j].buf;
                for (k = 0; k < f_alloc_lines[i].allocs[j].size; k++) {
                    if (&buf[k] == pointer) {
                        printf(DIM BOLD RED
                               "Trying to reallocate pointer %zu bytes (out of "
                               "%u) in to "
                               "allocation made in %s on line %u.\n" RESET,
                               k, f_alloc_lines[i].allocs[j].size,
                               f_alloc_lines[i].file, f_alloc_lines[i].line);

                        printf(RESET);
                    }
                }
            }
        }
        exit(0);
    }
    move = f_alloc_lines[i].allocs[j].size;

    if (move > size)
        move = size;

    pointer2 = malloc(size + F_MEMORY_OVER_ALLOC);
    if (pointer2 == NULL) {
        printf(DIM BOLD RED
               "MEMORY ERROR: Realloc returns NULL when trying to allocate %zu "
               "bytes "
               "at line %u in file %s\n" RESET,
               size, line, file);

        printf(RESET);
        if (f_alloc_mutex != NULL)
            f_alloc_mutex_unlock(f_alloc_mutex);
        f_debug_mem_print(0);
        exit(0);
    }
    for (i = 0; i < size + F_MEMORY_OVER_ALLOC; i++)
        ((unsigned char *)pointer2)[i] = F_MEMORY_MAGIC_NUMBER;
    memcpy(pointer2, pointer, move);

    f_debug_mem_add(pointer2, size, file, line);
    move = 0;
    f_debug_mem_remove(pointer, file, line, true, &move);
#ifdef F_MEMORY_PRINT
    printf(
        DIM BLUE
        "Realloc %zu bytes at pointer %p to %zu bytes at pointer %p at %s line "
        "%u\n" RESET,
        size, pointer, move, pointer2, file, line);

    printf(RESET);
#endif
    free(pointer);

    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return pointer2;
}

size_t f_debug_mem_footprint(unsigned int min_allocs) {
    unsigned int i, j;
    size_t size = 0;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++)
        size += f_alloc_lines[i].size;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return size;
}

bool f_debug_mem_query(void *pointer, unsigned int *line, char **file,
                       size_t *size) {
    unsigned int i, j;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++) {
        for (j = 0; j < f_alloc_lines[i].alloc_count; j++) {
            if (f_alloc_lines[i].allocs[j].buf == pointer) {
                if (line != NULL)
                    *line = f_alloc_lines[i].line;
                if (file != NULL)
                    *file = f_alloc_lines[i].file;
                if (size != NULL)
                    *size = f_alloc_lines[i].allocs[j].size;
                if (f_alloc_mutex != NULL)
                    f_alloc_mutex_unlock(f_alloc_mutex);
                return true;
            }
        }
    }
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return false;
}

bool f_debug_mem_test(void *pointer, size_t size, bool ignore_not_found) {
    unsigned int i, j;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++) {
        for (j = 0; j < f_alloc_lines[i].alloc_count; j++) {
            if (f_alloc_lines[i].allocs[j].buf >= pointer &&
                ((unsigned char *)f_alloc_lines[i].allocs[j].buf) +
                        f_alloc_lines[i].allocs[j].size <=
                    pointer) {
                if (((unsigned char *)f_alloc_lines[i].allocs[j].buf) +
                        f_alloc_lines[i].allocs[j].size <
                    ((unsigned char *)pointer) + size) {
                    printf(
                        DIM BOLD RED
                        "MEMORY ERROR: Not enough memory to access pointer %p, "
                        "%u bytes "
                        "missing\n" RESET,
                        pointer,
                        (unsigned int)(((unsigned char *)f_alloc_lines[i]
                                            .allocs[j]
                                            .buf) +
                                       f_alloc_lines[i].allocs[j].size) -
                            (unsigned int)(((unsigned char *)pointer) + size));

                    printf(RESET);
                    if (f_alloc_mutex != NULL)
                        f_alloc_mutex_unlock(f_alloc_mutex);
                    return true;
                } else {
                    if (f_alloc_mutex != NULL)
                        f_alloc_mutex_unlock(f_alloc_mutex);
                    return false;
                }
            }
        }
    }
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    if (ignore_not_found)
        return false;

    for (i = 0; i < f_freed_memory_count; i++) {
        if (pointer >= f_freed_memory[i].pointer &&
            ((unsigned char *)f_freed_memory[i].pointer) +
                    f_freed_memory[i].size >=
                ((unsigned char *)pointer) + size) {
            printf(DIM BOLD RED
                   "MEMORY ERROR: Pointer %p was freed on line %u in file "
                   "%s\n" RESET,
                   pointer, f_freed_memory[i].free_line,
                   f_freed_memory[i].free_file);

            printf(RESET);
        }
    }

    printf(DIM BOLD RED
           "MEMORY ERROR: No matching memory for pointer %p found!\n" RESET,
           pointer);

    printf(RESET);
    return true;
}

size_t f_debug_mem_consumption() {
    unsigned int i;
    size_t sum = 0;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
    for (i = 0; i < f_alloc_line_count; i++)
        sum += f_alloc_lines[i].size;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
    return sum;
}

void f_debug_mem_reset() {
    unsigned int i;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_lock(f_alloc_mutex);
#ifdef F_MEMORY_PRINT
    printf(DIM "Memory reset\n" RESET);

    printf(RESET);
#endif
    for (i = 0; i < f_alloc_line_count; i++)
        free(f_alloc_lines[i].allocs);
    f_alloc_line_count = 0;
    if (f_alloc_mutex != NULL)
        f_alloc_mutex_unlock(f_alloc_mutex);
}

void exit_crash(unsigned int i) {
    unsigned int *a = NULL;
    a[0] = 0;
}
