// xdbg.c

#include "xdbg.h"

static void xdbg_internal_alloc_check(void *pointer, const char *file,
                                      const unsigned int line,
                                      const char *function) {
    if (!pointer) {
        fprintf(stderr,
                "[%s%sERROR%s] Memory allocation failed on "
                "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
                AEC_RED, AEC_BOLD, AEC_RESET, AEC_CYAN, AEC_ITALIC, file,
                AEC_RESET, AEC_CYAN, AEC_ITALIC, line, AEC_RESET, AEC_CYAN,
                AEC_ITALIC, function, AEC_RESET);

        exit(EXIT_FAILURE);
    }
}

static bool isXDBGinitialized = false;

static inline void xdbg_initialize_check(const char *file,
                                         const unsigned int line,
                                         const char *function) {
    if (!isXDBGinitialized) {
        fprintf(
            stderr,
            "[%s%sERROR%s] Must call XDBG_INITIALIZE() first. Illegal call on "
            "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
            AEC_RED, AEC_BOLD, AEC_RESET, AEC_CYAN, AEC_ITALIC, file, AEC_RESET,
            AEC_CYAN, AEC_ITALIC, line, AEC_RESET, AEC_CYAN, AEC_ITALIC,
            function, AEC_RESET);

        exit(EXIT_FAILURE);
    }
}

struct xdbg_allocation_record {
    unsigned int total_allocations;
    unsigned int total_frees;
    unsigned int total_bytes;
};

static struct xdbg_allocation_record allocation_record;

struct xdbg_allocated_pointer {
    void *pointer;
    size_t size;
    const char *file;
    unsigned int line;
    const char *function;
    bool freed;
    struct xdbg_allocated_pointer *next;
};

static struct xdbg_allocated_pointer *allocated_pointer_head;
static struct xdbg_allocated_pointer *allocated_pointer_tail;

struct xdbg_freed_pointer_info {
    void *pointer;
    const char *file;
    unsigned int line;
    const char *function;
    size_t size;
    struct xdbg_freed_pointer_info *next;
};

static struct xdbg_freed_pointer_info *freed_pointers_head = NULL;

static void xdbg_check_freed_pointer(void *pointer, const char *file,
                                     unsigned int line, const char *function) {
    struct xdbg_freed_pointer_info *curr = freed_pointers_head;
    while (curr) {
        if (curr->pointer == pointer) {
            fprintf(
                stderr,
                "[%s%sERROR%s] Double free detected on "
                "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
                AEC_RED, AEC_BOLD, AEC_RESET, AEC_CYAN, AEC_ITALIC, file,
                AEC_RESET, AEC_CYAN, AEC_ITALIC, line, AEC_RESET, AEC_CYAN,
                AEC_ITALIC, function, AEC_RESET);
            exit(EXIT_FAILURE);
        }
        curr = curr->next;
    }
}

// TODO: optimize
#define MAX_BUFFER_SIZE 2048
static struct xdbg_freed_pointer_info xdbg_freed_pointer[MAX_BUFFER_SIZE];
static size_t xdbg_freed_pointer_index = 0;

void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                  const char *function) {
    void *pointer;
    struct xdbg_allocated_pointer *curr;
    xdbg_initialize_check(file, line, function);
    if (size >= SIZE_MAX) {
        fprintf(stderr,
                "[%s%sERROR%s] Allocation size of %s%lu%s bytes exceeds memory "
                "limit in "
                "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
                AEC_RED, AEC_BOLD, AEC_RESET, AEC_BLUE, size, AEC_RESET,
                AEC_CYAN, AEC_ITALIC, file, AEC_RESET, AEC_CYAN, AEC_ITALIC,
                line, AEC_RESET, AEC_CYAN, AEC_ITALIC, function, AEC_RESET);
        exit(EXIT_FAILURE);
    }
    pointer = malloc(size);
    xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);
    curr = malloc(sizeof(*curr));
    xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

    curr->pointer = pointer;
    curr->size = size;
    curr->file = file;
    curr->line = line;
    curr->function = function;
    curr->next = NULL;
    curr->freed = false;

    if (allocated_pointer_head == NULL) {
        allocated_pointer_head = curr;
        allocated_pointer_tail = allocated_pointer_head;
    } else {
        allocated_pointer_tail->next = curr;
        allocated_pointer_tail = allocated_pointer_tail->next;
    }
    allocation_record.total_allocations++;
    allocation_record.total_bytes += size;
    return pointer;
}

// FIX: handle overflow graciously
void *xdbg_calloc(size_t number, size_t size, const char *file,
                  unsigned int line, const char *function) {
    int *pointer;
    struct xdbg_allocated_pointer *curr;
    xdbg_initialize_check(file, line, function);

    // Special cases
    if (size == 0) {
        return NULL;
    }
    pointer = calloc(number, size);
    xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);

    curr = malloc(sizeof(*curr));
    xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

    curr->pointer = pointer;
    curr->size = size;
    curr->file = file;
    curr->line = line;
    curr->function = function;
    curr->next = NULL;
    curr->freed = false;

    if (allocated_pointer_head == NULL) {
        allocated_pointer_head = curr;
        allocated_pointer_tail = allocated_pointer_head;
    } else {
        allocated_pointer_tail->next = curr;
        allocated_pointer_tail = allocated_pointer_tail->next;
    }
    allocation_record.total_allocations++;
    allocation_record.total_bytes += size;
    return pointer;
}

void *xdbg_realloc(void *pointer, size_t size, const char *file,
                   unsigned int line, const char *function) {
    struct xdbg_allocated_pointer *curr = allocated_pointer_head;
    xdbg_initialize_check(file, line, function);

    // Special cases
    if (size == 0) {
        xdbg_free(pointer, file, line, function);
        return NULL;
    }
    if (pointer == NULL) {
        return xdbg_malloc(size, file, line, function);
    }

    // Find existing allocation
    while (curr) {
        if (curr->pointer == pointer && !curr->freed) {
            size_t old_size = curr->size;
            void *new_ptr = realloc(pointer, size);
            xdbg_internal_alloc_check(new_ptr, file, line, function);

            // Update allocation record
            allocation_record.total_bytes -= old_size;
            allocation_record.total_bytes += size;
            curr->pointer = new_ptr;
            curr->size = size;
            curr->file = file;
            curr->line = line;
            curr->function = function;
            return new_ptr;
        }
        curr = curr->next;
    }

    fprintf(stderr,
            "[%s%sERROR%s] Attempted to realloc an unknown pointer on "
            "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
            AEC_RED, AEC_BOLD, AEC_RESET, AEC_CYAN, AEC_ITALIC, file, AEC_RESET,
            AEC_CYAN, AEC_ITALIC, line, AEC_RESET, AEC_CYAN, AEC_ITALIC,
            function, AEC_RESET);
    exit(EXIT_FAILURE);
}

void xdbg_free(void *pointer, const char *file, unsigned int line,
               const char *function) {

    struct xdbg_allocated_pointer *prev = NULL;
    struct xdbg_allocated_pointer *curr = allocated_pointer_head;

    xdbg_initialize_check(file, line, function);
    xdbg_check_freed_pointer(pointer, file, line, function);

    // Find and remove the allocation record
    while (curr) {
        if (curr->pointer == pointer) {
            // Remove from list
            if (prev) {
                prev->next = curr->next;
            } else {
                allocated_pointer_head = curr->next;
            }
            if (curr == allocated_pointer_tail) {
                allocated_pointer_tail = prev;
            }

            // Store freed pointer info
            if (xdbg_freed_pointer_index < MAX_BUFFER_SIZE) {
                xdbg_freed_pointer[xdbg_freed_pointer_index].pointer = pointer;
                xdbg_freed_pointer[xdbg_freed_pointer_index].file = file;
                xdbg_freed_pointer[xdbg_freed_pointer_index].line = line;
                xdbg_freed_pointer[xdbg_freed_pointer_index].function =
                    function;
                xdbg_freed_pointer[xdbg_freed_pointer_index].size = curr->size;
                xdbg_freed_pointer_index++;
            }

            free(pointer);
            free(curr); // Free the tracking node
            allocation_record.total_frees++;
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    // If we get here, pointer wasn't found
    fprintf(stderr,
            "[%s%sERROR%s] Pointer %s%p%s was never allocated and cannot "
            "be freed in "
            "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
            AEC_RED, AEC_BOLD, AEC_RESET, AEC_BLUE, pointer, AEC_RESET,
            AEC_CYAN, AEC_ITALIC, file, AEC_RESET, AEC_CYAN, AEC_ITALIC, line,
            AEC_RESET, AEC_CYAN, AEC_ITALIC, function, AEC_RESET);
    exit(EXIT_FAILURE);
}

static inline void
xdbg_internal_pointer_print_format(struct xdbg_allocated_pointer *pointer) {
    printf("[%sCall Location%s] %s%sfile: %s, line: %u, function: %s%s\n",
           AEC_MAGENTA, AEC_RESET, AEC_CYAN, AEC_ITALIC, pointer->file,
           pointer->line, pointer->function, AEC_RESET);

    printf("[%sPointer Address%s] %s%s%p%s\n", AEC_MAGENTA, AEC_RESET, AEC_BLUE,
           AEC_BOLD, (void *)pointer->pointer, AEC_RESET);

    printf("[%sAllocated Bytes%s] %s%s%lu%s\n", AEC_MAGENTA, AEC_RESET,
           AEC_BLUE, AEC_BOLD, pointer->size, AEC_RESET);

    printf("[%sPointer Freed%s] %s%s%s%s\n", AEC_MAGENTA, AEC_RESET, AEC_BLUE,
           AEC_BOLD, (pointer->freed ? "true" : "false"), AEC_RESET);
}

void xdbg_report(const char *file, unsigned int line, const char *function) {

    struct xdbg_allocated_pointer *show_all_pointers_head =
        allocated_pointer_head;
    xdbg_initialize_check(file, line, function);
    while (show_all_pointers_head != NULL) {
        struct xdbg_allocated_pointer *show_all_pointers_next =
            show_all_pointers_head->next;
        xdbg_internal_pointer_print_format(show_all_pointers_head);
        show_all_pointers_head = show_all_pointers_next;
    }
    puts("--------------------------------------------------");
    printf("[%sTotal Allocations%s] %s%s%u%s\n", AEC_MAGENTA, AEC_RESET,
           AEC_BLUE, AEC_BOLD, allocation_record.total_allocations, AEC_RESET);
    printf("[%sTotal Frees%s] %s%s%u%s\n", AEC_MAGENTA, AEC_RESET, AEC_BLUE,
           AEC_BOLD, allocation_record.total_frees, AEC_RESET);
    printf("[%sTotal Bytes%s] %s%s%u%s\n", AEC_MAGENTA, AEC_RESET, AEC_BLUE,
           AEC_BOLD, allocation_record.total_bytes, AEC_RESET);
}

void xdbg_initialize(const char *file, unsigned int line,
                     const char *function) {
    memset(&allocation_record, 0, sizeof(allocation_record));

    if (isXDBGinitialized) {
        fprintf(stderr,
                "[%s%sERROR%s] XDBG_INITIALIZE() called twice on "
                "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
                AEC_RED, AEC_BOLD, AEC_RESET, AEC_CYAN, AEC_ITALIC, file,
                AEC_RESET, AEC_CYAN, AEC_ITALIC, line, AEC_RESET, AEC_CYAN,
                AEC_ITALIC, function, AEC_RESET);

        exit(EXIT_FAILURE);
    }
    allocated_pointer_head = NULL;
    allocated_pointer_tail = NULL;
    isXDBGinitialized = true;
}

void xdbg_finalize(const char *file, unsigned int line, const char *function) {
    struct xdbg_allocated_pointer *allocated_pointer_finalize =
        allocated_pointer_head;
    xdbg_initialize_check(file, line, function);
    while (allocated_pointer_finalize != NULL) {
        struct xdbg_allocated_pointer *allocated_pointer_next =
            allocated_pointer_finalize->next;
        free(allocated_pointer_finalize);
        allocated_pointer_finalize = allocated_pointer_next;
    }
    isXDBGinitialized = false;
}
