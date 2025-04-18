// xdbg.c

#include "xdbg.h"

#define IMD_MEMORY_DEBUG // enables the internal memory debugger
#include "imd.h"

static void xdbg_internal_alloc_check(void *pointer, const char *file,
                                      const unsigned int line,
                                      const char *function) {
    if (!pointer) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
}

static bool isXDBGinitialized = false;

static inline void xdbg_initialize_check(const char *file,
                                         const unsigned int line,
                                         const char *function) {
    if (!isXDBGinitialized) {
        fprintf(stderr, "Must call xdbg_initialize() first.\n");
        exit(EXIT_FAILURE);
    }
}

struct xdbg_allocation_record {
    unsigned int total_allocations;
    unsigned int total_frees;
    unsigned int total_bytes;
};

struct xdbg_allocation_record allocation_record = {0, 0, 0};

enum memory_function_type {
    MEMORY_FUNCTION_MALLOC,
    MEMORY_FUNCTION_REALLOC,
    MEMORY_FUNCTION_CALLOC,
    MEMORY_FUNCTION_FREE
};

struct xdbg_allocated_pointer {
    void *pointer; // pointer to be allocated/freed
    size_t size;   // size of the allocated memory in bytes
    enum memory_function_type memory_function; // type of memory function called
    const char *file;  // name of file the pointer was allocated/freed
    unsigned int line; // line number of the allocation/free
    const char
        *function; // function name of where the pointer was allocated/freed
    bool freed;
    struct xdbg_allocated_pointer *next; // address of the next struct
    // struct xdbg_allocated_pointer *prev; // TODO: address of the previous
    // struct
};

struct xdbg_allocated_pointer *allocated_pointer_head;
struct xdbg_allocated_pointer *allocated_pointer_tail;

struct xdbg_freed_pointer_info {
    void *pointer;        // pointer to be freed
    const char *file;     // name of file the pointer was freed
    unsigned int line;    // line number of the free
    const char *function; // function name of where the pointer was freed
    // size_t size;          // TODO: size of the allocated memory in bytes
};

// TODO: optimize
#define MAX_BUFFER_SIZE 1024
struct xdbg_freed_pointer_info xdbg_freed_pointer[MAX_BUFFER_SIZE];
size_t xdbg_freed_pointer_index = 0;

void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                  const char *function) {
    xdbg_initialize_check(file, line, function);
    if (size < 0 || size >= SIZE_MAX) {
        fprintf(stderr,
                "[%s%sERROR%s] Allocation size of %s%lu%s bytes exceeds memory "
                "limit in "
                "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
                XDBG_ANSI_RED, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_BLUE,
                size, XDBG_ANSI_RESET, XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC, file,
                XDBG_ANSI_RESET, XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC, line,
                XDBG_ANSI_RESET, XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC, function,
                XDBG_ANSI_RESET);
        // return NULL; // for testing
        exit(EXIT_FAILURE);
    }
    void *pointer = malloc(size);
    xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);
    struct xdbg_allocated_pointer *curr = malloc(sizeof(*curr));
    xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

    curr->pointer = pointer;
    curr->size = size;
    curr->memory_function = MEMORY_FUNCTION_MALLOC;
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

void *xdbg_calloc(size_t number, size_t size, const char *file,
                  unsigned int line, const char *function) {
    xdbg_initialize_check(file, line, function);

    int *pointer = calloc(number, size);
    xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);

    struct xdbg_allocated_pointer *curr = malloc(sizeof(*curr));
    xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

    curr->pointer = pointer;
    curr->size = size;
    curr->memory_function = MEMORY_FUNCTION_CALLOC;
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

// TODO: rewrite realloc
void *xdbg_realloc(void *pointer, size_t size, const char *file,
                   unsigned int line, const char *function) {
    xdbg_initialize_check(file, line, function);

    pointer = realloc(pointer, size);
    if (size == 0 && pointer != NULL)
        pointer = NULL;
    else
        xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);

    struct xdbg_allocated_pointer *curr = malloc(sizeof(*curr));
    xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

    curr->pointer = pointer;
    curr->size = size;
    curr->memory_function = MEMORY_FUNCTION_REALLOC;
    curr->file = file;
    curr->line = line;
    curr->function = function;
    curr->next = NULL;
    curr->freed = false;

    // FIXME: not tracking correctly

    // if (!pointer) {
    //   return xdbg_malloc(size, __FILE__, __LINE__, __func__);
    // } else if (size == 0) {
    //   xdbg_free(pointer, __FILE__, __LINE__, __func__);
    //   return NULL;
    // }

    if (size == 0 && !pointer) {
        allocation_record.total_frees++;
    } else if (pointer == NULL) {
        allocation_record.total_allocations++;
    } else {
        // allocation_record.total_allocations++;
        // allocation_record.total_frees++;
    }

    if (allocated_pointer_head == NULL) {
        allocated_pointer_head = curr;
        allocated_pointer_tail = allocated_pointer_head;
    } else {
        allocated_pointer_tail->next = curr;
        allocated_pointer_tail = allocated_pointer_tail->next;
    }
    return pointer;
}

static void xdbg_check_freed_pointer(void *pointer) {
    size_t i;
    for (i = 0; i < xdbg_freed_pointer_index; i++) {
        if (xdbg_freed_pointer[i].pointer == pointer) {
            fprintf(
                stderr,
                "[%s%sERROR%s] %s%p%s was freed in "
                "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
                XDBG_ANSI_RED, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_BLUE,
                pointer, XDBG_ANSI_RESET, XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC,
                xdbg_freed_pointer[i].file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
                XDBG_ANSI_ITALIC, xdbg_freed_pointer[i].line, XDBG_ANSI_RESET,
                XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC,
                xdbg_freed_pointer[i].function, XDBG_ANSI_RESET);
            return; // for testing
            // exit(EXIT_FAILURE);
        }
    }
}

void xdbg_free(void *pointer, const char *file, unsigned int line,
               const char *function) {
    xdbg_initialize_check(file, line, function);
    xdbg_check_freed_pointer(pointer);
    struct xdbg_allocated_pointer *curr = malloc(sizeof(*curr));
    xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

    /*FIXME:Invalid free, out-of-bounds detection
     * currently just seg faults*/

    curr->pointer = pointer;
    curr->size = 0;
    curr->memory_function = MEMORY_FUNCTION_FREE;
    curr->file = file;
    curr->line = line;
    curr->function = function;
    curr->next = NULL;
    curr->freed = true;
    if (pointer != NULL) {
        printf("[%s%sSUCCESS%s] Pointer %s%p%s "
               "was freed in "
               "file %s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
               XDBG_ANSI_GREEN, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_BLUE,
               (void *)pointer, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
               XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
               XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
               XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);

        struct xdbg_freed_pointer_info freed_pointer;
        freed_pointer.pointer = pointer;
        freed_pointer.file = file;
        freed_pointer.line = line;
        freed_pointer.function = function;
        xdbg_freed_pointer[xdbg_freed_pointer_index++] = freed_pointer;
        free(pointer);
        pointer = NULL;
    } else {
        printf("[%s%sSUCCESS%s] Pointer %s%p%s was freed in file "
               "%s%s%s%s, on line %s%s%u%s, within function %s%s%s%s.\n",
               XDBG_ANSI_GREEN, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_BLUE,
               (void *)pointer, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
               XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
               XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
               XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);
    }

    if (allocated_pointer_head == NULL) {
        allocated_pointer_head = curr;
        allocated_pointer_tail = allocated_pointer_head;
    } else {
        allocated_pointer_tail->next = curr;
        allocated_pointer_tail = allocated_pointer_tail->next;
    }
    allocation_record.total_frees++;
}

static inline void
xdbg_internal_pointer_print_format(struct xdbg_allocated_pointer *pointer) {
    char *memory_function_literal;
    switch (pointer->memory_function) {
    case MEMORY_FUNCTION_MALLOC:
        memory_function_literal = "malloc\0";
        break;
    case MEMORY_FUNCTION_CALLOC:
        memory_function_literal = "calloc\0";
        break;
    case MEMORY_FUNCTION_REALLOC:
        memory_function_literal = "realloc\0";
        break;
    case MEMORY_FUNCTION_FREE:
        memory_function_literal = "free\0";
        break;
    default:
        memory_function_literal = "\0";
        break;
    }

    printf("[%sCall Location%s] %s%sfile: %s, line: %u, function: %s%s\n",
           XDBG_ANSI_MAGENTA, XDBG_ANSI_RESET, XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC,
           pointer->file, pointer->line, pointer->function, XDBG_ANSI_RESET);

    printf("[%sMemory Function%s] %s%s%s%s\n", XDBG_ANSI_MAGENTA,
           XDBG_ANSI_RESET, XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
           memory_function_literal, XDBG_ANSI_RESET);

    printf("[%sPointer Address%s] %s%s%p%s\n", XDBG_ANSI_MAGENTA,
           XDBG_ANSI_RESET, XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
           (void *)pointer->pointer, XDBG_ANSI_RESET);

    printf("[%sAllocated Bytes%s] %s%s%lu%s\n", XDBG_ANSI_MAGENTA,
           XDBG_ANSI_RESET, XDBG_ANSI_BLUE, XDBG_ANSI_BOLD, pointer->size,
           XDBG_ANSI_RESET);

    printf("[%sPointer Freed%s] %s%s%s%s\n", XDBG_ANSI_MAGENTA, XDBG_ANSI_RESET,
           XDBG_ANSI_BLUE, XDBG_ANSI_BOLD, (pointer->freed ? "true" : "false"),
           XDBG_ANSI_RESET);
}

void xdbg_report(const char *file, unsigned int line, const char *function) {
    xdbg_initialize_check(file, line, function);
    putchar('\n');
    printf(
        "[%s%sSUCCESS%s] XDBG memory report called in file %s%s%s%s, on line "
        "%s%s%u%s, within function %s%s%s%s.\n",
        XDBG_ANSI_GREEN, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
        XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
        XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
        XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);
    putchar('\n');

    struct xdbg_allocated_pointer *show_all_pointers_head =
        allocated_pointer_head;
    while (show_all_pointers_head != NULL) {
        struct xdbg_allocated_pointer *show_all_pointers_next =
            show_all_pointers_head->next;
        xdbg_internal_pointer_print_format(show_all_pointers_head);
        putchar('\n');
        show_all_pointers_head = show_all_pointers_next;
    }
    printf("[%sTotal Allocations%s] %s%s%u%s\n", XDBG_ANSI_MAGENTA,
           XDBG_ANSI_RESET, XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
           allocation_record.total_allocations, XDBG_ANSI_RESET);
    printf("[%sTotal Frees%s] %s%s%u%s\n", XDBG_ANSI_MAGENTA, XDBG_ANSI_RESET,
           XDBG_ANSI_BLUE, XDBG_ANSI_BOLD, allocation_record.total_frees,
           XDBG_ANSI_RESET);
    printf("[%sTotal Bytes%s] %s%s%u%s\n", XDBG_ANSI_MAGENTA, XDBG_ANSI_RESET,
           XDBG_ANSI_BLUE, XDBG_ANSI_BOLD, allocation_record.total_bytes,
           XDBG_ANSI_RESET);

    putchar('\n');
}

void xdbg_initialize(const char *file, unsigned int line,
                     const char *function) {
    putchar('\n');
    printf("[%s%sWARNING%s] XDBG initializing in file %s%s%s%s, on line "
           "%s%s%u%s, within function %s%s%s%s.\n",
           XDBG_ANSI_YELLOW, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);
    putchar('\n');

    if (isXDBGinitialized) {
        fprintf(stderr, "XDGB_initialize() was already called.\n");
        exit(EXIT_FAILURE);
    }
    allocated_pointer_head = NULL;
    allocated_pointer_tail = NULL;
    isXDBGinitialized = true;
    putchar('\n');
    printf("[%s%sSUCCESS%s] XDBG initialized in file %s%s%s%s, on line "
           "%s%s%u%s, within function %s%s%s%s.\n",
           XDBG_ANSI_GREEN, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);
    putchar('\n');
}

void xdbg_finalize(const char *file, unsigned int line, const char *function) {
    printf("[%s%sWARNING%s] XDBG finalizing in file %s%s%s%s, on line "
           "%s%s%u%s, within function %s%s%s%s.\n",
           XDBG_ANSI_YELLOW, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);
    putchar('\n');
    xdbg_initialize_check(file, line, function);
    struct xdbg_allocated_pointer *allocated_pointer_finalize =
        allocated_pointer_head;
    while (allocated_pointer_finalize != NULL) {
        struct xdbg_allocated_pointer *allocated_pointer_next =
            allocated_pointer_finalize->next;
        free(allocated_pointer_finalize);
        allocated_pointer_finalize = allocated_pointer_next;
    }
    isXDBGinitialized = false;
    putchar('\n');
    printf("[%s%sSUCCESS%s] XDBG finalized in file %s%s%s%s, on line "
           "%s%s%u%s, within function %s%s%s%s.\n",
           XDBG_ANSI_GREEN, XDBG_ANSI_BOLD, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, file, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, line, XDBG_ANSI_RESET, XDBG_ANSI_CYAN,
           XDBG_ANSI_ITALIC, function, XDBG_ANSI_RESET);
    putchar('\n');
}

// TODO:
void xdbg_report_leaks(void) {
    // Traverse linked list of allocations and print unfreed ones
}

// TODO:
void xdbg_reset_tracking(void) {
    // Frees all internal tracking structures (not user allocations!)
    // For testing purposes only
}

// TODO: Move to separate testing file
//
/******************************************************************************/
/*                                                        TESTING DECLARATIONS*/
/******************************************************************************/

static void test_malloc(void);
static void test_realloc(void);
static void test_realloc(void);
static void test_free(void);

// TODO: Move to separate example file
//
/******************************************************************************/
/*                                                                       MAIN */
/******************************************************************************/

int main(void) {

    XDBG_INITIALIZE();

    // int *a = xdbg_malloc(10 * sizeof(*a), __FILE__, __LINE__, __func__);
    // int *b = xdbg_malloc(10 * sizeof(*b), __FILE__, __LINE__, __func__);
    // int *c = xdbg_malloc(10 * sizeof(*c), __FILE__, __LINE__, __func__);
    // int *d = xdbg_malloc(10 * sizeof(*d), __FILE__, __LINE__, __func__);
    // int *e = xdbg_malloc(10 * sizeof(*e), __FILE__, __LINE__, __func__);
    // a[0] = 1000;
    // e[3] = 1000;
    // c[2] = 1000;
    // xdbg_free(a, __FILE__, __LINE__, __func__);
    // xdbg_free(b, __FILE__, __LINE__, __func__);
    // xdbg_free(c, __FILE__, __LINE__, __func__);
    // xdbg_free(d, __FILE__, __LINE__, __func__);
    // xdbg_free(e, __FILE__, __LINE__, __func__);

    // test_malloc(); // PASSED
    test_free();
    // test_realloc();
    // test_calloc();

    XDBG_REPORT();
    XDBG_FINALIZE();

    printf("XDBG TESTS ENDED\n");

    imd_debug_memory_init(NULL, NULL, NULL);
    imd_debug_memory_print(0);
    imd_debug_memory_reset();

    return 0;
}

// TODO: Move to separate testing file
//
/******************************************************************************/
/*                                                         TESTING DEFINITION */
/******************************************************************************/

// Summary of Malloc Test Cases:
// Case     Description
// 1	Normal allocation
// 2	Allocation with size = 0
// 3	Huge allocation (OOM)
// 4	Leak detection (intentional no free)
static void test_malloc(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    // Case 1: normal allocation
    printf("Case 1: normal malloc\n");
    int *a1 = xdbg_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (a1) {
        a1[0] = 123;
        printf("Value stored in malloc'd memory: %d\n", a1[0]);
        xdbg_free(a1, __FILE__, __LINE__, __func__);
    }

    // Case 2: malloc 0 bytes
    printf("Case 2: malloc 0 bytes (should return NULL or valid unique ptr)\n");
    int *a2 = xdbg_malloc(0, __FILE__, __LINE__, __func__);
    if (a2) {
        printf("malloc(0) returned non-NULL\n");
        xdbg_free(a2, __FILE__, __LINE__, __func__);
    }

    // Case 3: malloc very large size (simulate OOM)
    printf("Case 3: malloc large size to simulate OOM\n");
    void *a3 = xdbg_malloc((size_t)-1, __FILE__, __LINE__, __func__);
    if (!a3) {
        printf("malloc failed as expected for large size\n");
    }

    // Case 4: malloc and forget to free (leak detection test)
    printf("Case 4: malloc without free (intentional leak)\n");
    void *a4 = xdbg_malloc(64, __FILE__, __LINE__, __func__);
    (void)a4; // intentionally not freeing

    printf("Finished test_malloc()\n");
}

// Summary of Calloc Test Cases:
// Case     Description
// 1	Normal zero-initialized memory
// 2	calloc(0, size)
// 3	calloc(n, 0)
// 4	Overflow detection
static void test_calloc(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    // Case 1: normal calloc
    printf("Case 1: normal calloc\n");
    int *c1 = xdbg_calloc(10, sizeof(int), __FILE__, __LINE__, __func__);
    if (c1) {
        int initialized = 1;
        for (int i = 0; i < 10; i++) {
            if (c1[i] != 0)
                initialized = 0;
        }
        printf("Calloc initialized to zero: %s\n", initialized ? "Yes" : "No");
        xdbg_free(c1, __FILE__, __LINE__, __func__);
    }

    // Case 2: calloc with 0 elements
    printf("Case 2: calloc with 0 elements\n");
    int *c2 = xdbg_calloc(0, sizeof(int), __FILE__, __LINE__, __func__);
    if (c2) {
        printf("calloc(0, size) returned non-NULL\n");
        xdbg_free(c2, __FILE__, __LINE__, __func__);
    }

    // Case 3: calloc with 0 size
    printf("Case 3: calloc with 0 size\n");
    int *c3 = xdbg_calloc(10, 0, __FILE__, __LINE__, __func__);
    if (c3) {
        printf("calloc(n, 0) returned non-NULL\n");
        xdbg_free(c3, __FILE__, __LINE__, __func__);
    }

    // Case 4: calloc overflow simulation (n * size too big)
    printf("Case 4: calloc with size overflow\n");
    size_t huge = (size_t)1 << (sizeof(size_t) * 4); // simulate overflow
    void *c4 = xdbg_calloc(huge, huge, __FILE__, __LINE__, __func__);
    if (!c4) {
        printf("Calloc failed as expected due to overflow\n");
    }

    printf("Finished test_calloc()\n");
}

// Summary of Free Test Cases:
// Case     Description
// 1	Normal free
// 2	Double free
// 3	Free NULL
// 4	Free invalid pointer
static void test_free(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    // Case 1: normal malloc + free
    printf("Case 1: malloc then free\n");
    int *f1 = xdbg_malloc(20 * sizeof(int), __FILE__, __LINE__, __func__);
    if (f1) {
        xdbg_free(f1, __FILE__, __LINE__, __func__);
    }

    // Case 2: double free
    printf("Case 2: double free (should be detected)\n");
    int *f2 = xdbg_malloc(32, __FILE__, __LINE__, __func__);
    if (f2) {
        xdbg_free(f2, __FILE__, __LINE__, __func__);
        xdbg_free(f2, __FILE__, __LINE__,
                  __func__); // UB, but we test detection
    }

    // Case 3: free NULL pointer (should be safe)
    printf("Case 3: free NULL pointer (safe)\n");
    int *f3 = NULL;
    xdbg_free(f3, __FILE__, __LINE__, __func__);

    // Case 4: free unallocated pointer (should error)
    printf("Case 4: free unallocated pointer (intentional bad free)\n");
    int dummy = 0;
    xdbg_free(&dummy, __FILE__, __LINE__,
              __func__); // should be detected as invalid

    printf("Finished test_free()\n");
}

// Summary of Realloc Test Cases:
// Case     Description
// 1        Realloc to a larger size
// 2        Realloc to a smaller size
// 3        Realloc from NULL (acts like malloc)
// 4        Realloc to size 0 (acts like free)
// 5        Realloc after free (UB, but test if it’s handled)
// 6        Realloc to extremely large size to simulate OOM
static void test_realloc(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    // Case 1: realloc to larger size
    int *arr1 = xdbg_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr1) {
        printf("Case 1: realloc to larger size\n");
        arr1 =
            xdbg_realloc(arr1, 20 * sizeof(int), __FILE__, __LINE__, __func__);
        if (!arr1) {
            printf("Case 1 failed: realloc returned NULL\n");
        }
        xdbg_free(arr1, __FILE__, __LINE__, __func__);
    }

    // Case 2: realloc to smaller size
    int *arr2 = xdbg_malloc(20 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr2) {
        printf("Case 2: realloc to smaller size\n");
        arr2 =
            xdbg_realloc(arr2, 5 * sizeof(int), __FILE__, __LINE__, __func__);
        if (!arr2) {
            printf("Case 2 failed: realloc returned NULL\n");
        }
        xdbg_free(arr2, __FILE__, __LINE__, __func__);
    }

    // Case 3: realloc NULL pointer (should behave like malloc)
    printf("Case 3: realloc with NULL pointer (malloc behavior)\n");
    int *arr3 =
        xdbg_realloc(NULL, 15 * sizeof(int), __FILE__, __LINE__, __func__);
    if (!arr3) {
        printf("Case 3 failed: realloc returned NULL\n");
    }
    xdbg_free(arr3, __FILE__, __LINE__, __func__);

    // Case 4: realloc to size 0 (should behave like free)
    int *arr4 = xdbg_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr4) {
        printf("Case 4: realloc to size 0 (should free memory)\n");
        arr4 = xdbg_realloc(arr4, 0, __FILE__, __LINE__, __func__);
        if (arr4 != NULL) {
            printf("Case 4 failed: realloc(size=0) did not return NULL\n");
        }
    }

    // Case 5: realloc a freed pointer (undefined behavior, should be handled)
    printf("Case 5: realloc a freed pointer (should not be allowed)\n");
    int *arr5 = xdbg_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr5) {
        xdbg_free(arr5, __FILE__, __LINE__, __func__);
        arr5 =
            xdbg_realloc(arr5, 20 * sizeof(int), __FILE__, __LINE__, __func__);
        if (arr5) {
            printf("Case 5 warning: realloc on freed pointer succeeded (UB)\n");
            xdbg_free(arr5, __FILE__, __LINE__,
                      __func__); // avoid leak if somehow worked
        }
    }

    // Case 6: realloc very large size (simulate OOM)
    printf("Case 6: realloc with large size to simulate failure\n");
    int *arr6 = xdbg_malloc(100 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr6) {
        arr6 = xdbg_realloc(arr6, (size_t)-1, __FILE__, __LINE__, __func__);
        if (!arr6) {
            printf("Case 6 passed: realloc failed gracefully\n");
        } else {
            printf("Case 6 warning: large realloc unexpectedly succeeded\n");
            xdbg_free(arr6, __FILE__, __LINE__, __func__);
        }
    }

    printf("Finished test_realloc()\n");
}
