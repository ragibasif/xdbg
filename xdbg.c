#include "xdbg.h"

/************************************************************************/
/*INTERNAL DEBUGGING*/
/************************************************************************/

// #define IMD_MEMORY_DEBUG /* turns on the memory debugging system */
#include "imd.h"

#ifdef IMD_MEMORY_DEBUG
static void xdbg_internal_memory_debug(void) {
  imd_debug_memory_init(NULL, NULL, NULL);
  imd_debug_memory_print(0);
  imd_debug_memory_reset();
}
#endif // INTERNAL_MEMORY_DEBUG
/************************************************************************/

static inline void xdbg_print_prefix(void) {
  printf("(%s%s%s%s) ", XDBG_ANSI_MAGENTA, XDBG_ANSI_BOLD, "xdbg",
         XDBG_ANSI_RESET);
}

#define XDBG_INTERNAL_ERROR(msg, file, line, function)                         \
  xdbg_internal_log_wrapper(msg, "ERROR\0", XDBG_ANSI_RED, file, line, function)
#define XDBG_INTERNAL_SUCCESS(msg, file, line, function)                       \
  xdbg_internal_log_wrapper(msg, "SUCCESS\0", XDBG_ANSI_GREEN, file, line,     \
                            function)
#define XDBG_INTERNAL_WARNING(msg, file, line, function)                       \
  xdbg_internal_log_wrapper(msg, "WARNING\0", XDBG_ANSI_YELLOW, file, line,    \
                            function)

static inline void
xdbg_internal_log_wrapper(const char *message, const char *prefix,
                          const char *color, const char *file,
                          const unsigned int line, const char *function) {

  xdbg_print_prefix();
  printf("[%s%s%s%s] in (%s%s%s %u %s%s) %s\n", color, XDBG_ANSI_BOLD, prefix,
         XDBG_ANSI_RESET, XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC, file, line,
         function, XDBG_ANSI_RESET, message);
}

static void xdbg_internal_alloc_check(void *pointer, const char *file,
                                      const unsigned int line,
                                      const char *function) {
  if (!pointer) {
    XDBG_INTERNAL_ERROR("Memory allocation failed!", file, line, function);
    exit(EXIT_FAILURE);
  }
}

static bool isXDBGinitialized = false;

static inline void xdbg_init_check(const char *file, const unsigned int line,
                                   const char *function) {
  if (!isXDBGinitialized) {
    XDBG_INTERNAL_ERROR("Must call xdbg_init() first.", file, line, function);
    exit(EXIT_FAILURE);
  }
}

struct xdbg_allocation_record {
  unsigned int total_allocations;
  unsigned int total_frees;
};

struct xdbg_allocation_record allocation_record = {0, 0};

enum memory_function_type {
  MEMORY_FUNCTION_MALLOC,
  MEMORY_FUNCTION_REALLOC,
  MEMORY_FUNCTION_CALLOC,
  MEMORY_FUNCTION_FREE
};

struct xdbg_allocated_pointer {
  void *pointer; // address of the pointer
  size_t size;   // size of the allocated memory in bytes
  enum memory_function_type memory_function; // type of memory function called
  const char *file;  // name of file the pointer was allocated/freed
  unsigned int line; // line number of the allocation/free
  const char
      *function; // function name of where the pointer was allocated/freed
  bool freed;
  unsigned int total_allocs;           // total number of allocations so far
  unsigned int total_frees;            // total number of frees so far
  struct xdbg_allocated_pointer *next; // the address of the next struct
  /*struct xdbg_allocated_pointer *prev; // the address of the previous struct*/
};

struct xdbg_allocated_pointer *allocated_pointer_head;
struct xdbg_allocated_pointer *allocated_pointer_tail;

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

  xdbg_print_prefix();
  printf("[Call Location] %s%s%s %u %s%s\n", XDBG_ANSI_CYAN, XDBG_ANSI_ITALIC,
         pointer->file, pointer->line, pointer->function, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Allocation Function] %s%s%s%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         memory_function_literal, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Pointer Address] %s%s%p%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         (void *)pointer->pointer, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Allocated Bytes] %s%s%lu%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         pointer->size, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Pointer Freed] %s%s%u%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         pointer->freed, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Total Allocations] %s%s%u%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         pointer->total_allocs, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Total Frees] %s%s%u%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         pointer->total_frees, XDBG_ANSI_RESET);
}

void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                  const char *function) {
  xdbg_init_check(file, line, function);

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
  curr->total_allocs = allocation_record.total_allocations;
  curr->total_frees = allocation_record.total_frees;
  return pointer;
}

void *xdbg_calloc(size_t number, size_t size, const char *file,
                  unsigned int line, const char *function) {
  xdbg_init_check(file, line, function);

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
  curr->total_allocs = allocation_record.total_allocations;
  curr->total_frees = allocation_record.total_frees;
  return pointer;
}

void *xdbg_realloc(void *pointer, size_t size, const char *file,
                   unsigned int line, const char *function) {
  xdbg_init_check(file, line, function);

  // FIXME: not tracking correctly

  // if (!pointer) {
  //   return xdbg_malloc(size, __FILE__, __LINE__, __func__);
  // } else if (size == 0) {
  //   xdbg_free(pointer, __FILE__, __LINE__, __func__);
  //   return NULL;
  // }

  if (size == 0) {
    allocation_record.total_frees++;
  }
  if (pointer == NULL) {
    allocation_record.total_allocations++;
  } else {
    // allocation_record.total_allocations++;
    // allocation_record.total_frees++;
  }

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

  if (allocated_pointer_head == NULL) {
    allocated_pointer_head = curr;
    allocated_pointer_tail = allocated_pointer_head;
  } else {
    allocated_pointer_tail->next = curr;
    allocated_pointer_tail = allocated_pointer_tail->next;
  }
  curr->total_allocs = allocation_record.total_allocations;
  curr->total_frees = allocation_record.total_frees;
  return pointer;
}

static void *xdbg_find_pointer(void *pointer, const char *file,
                               unsigned int line, const char *function) {
  struct xdbg_allocated_pointer *curr = allocated_pointer_head;
  while (curr) {
    if (curr->pointer == pointer) {
      if (curr->freed == true) {
        XDBG_INTERNAL_ERROR("Attempted to free an already freed pointer.", file,
                            line, function);
        exit(EXIT_FAILURE);
      }
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

void xdbg_free(void *pointer, const char *file, unsigned int line,
               const char *function) {
  xdbg_init_check(file, line, function);
  // if (pointer == NULL) {
  //   XDBG_INTERNAL_ERROR("Attempted to free a NULL pointer.", file, line,
  //                       function);
  //   exit(EXIT_FAILURE);
  // }
  struct xdbg_allocated_pointer *curr = malloc(sizeof(*curr));
  xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);
  /*FIXME: Will need to check if pointer was already freed by iterating through
   * the list and checking the pointer address. Currently, the it will just
   * create a new node with the updated status. This fails if the pointer was
   * double freed.*/
  //
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
  // if (pointer != NULL) {
  //   free(pointer);
  //   pointer = NULL;
  // }

  if (allocated_pointer_head == NULL) {
    allocated_pointer_head = curr;
    allocated_pointer_tail = allocated_pointer_head;
  } else {
    allocated_pointer_tail->next = curr;
    allocated_pointer_tail = allocated_pointer_tail->next;
  }
  allocation_record.total_frees++;
  curr->total_allocs = allocation_record.total_allocations;
  curr->total_frees = allocation_record.total_frees;
}

void xdbg_report(const char *file, unsigned int line, const char *function) {
  xdbg_init_check(file, line, function);
  putchar('\n');
  xdbg_print_prefix();
  puts("XDBG Memory Report\n");
  struct xdbg_allocated_pointer *show_all_pointers_head =
      allocated_pointer_head;
  while (show_all_pointers_head != NULL) {
    struct xdbg_allocated_pointer *show_all_pointers_next =
        show_all_pointers_head->next;
    xdbg_internal_pointer_print_format(show_all_pointers_head);
    putchar('\n');
    show_all_pointers_head = show_all_pointers_next;
  }
}

void xdbg_init(const char *file, unsigned int line, const char *function) {
  if (isXDBGinitialized) {
    XDBG_INTERNAL_ERROR("XDGB_INIT() was already called.", file, line,
                        function);
    exit(EXIT_FAILURE);
  }
  XDBG_INTERNAL_WARNING("Initializing XDBG.", file, line, function);
  allocated_pointer_head = NULL;
  allocated_pointer_tail = NULL;
  isXDBGinitialized = true;
  XDBG_INTERNAL_SUCCESS("XDBG Initialized.", file, line, function);
}

void xdbg_clear(const char *file, unsigned int line, const char *function) {
  xdbg_init_check(file, line, function);
  XDBG_INTERNAL_WARNING("Clearing XDBG.", file, line, function);
  struct xdbg_allocated_pointer *allocated_pointer_clear =
      allocated_pointer_head;
  while (allocated_pointer_clear != NULL) {
    struct xdbg_allocated_pointer *allocated_pointer_next =
        allocated_pointer_clear->next;
    free(allocated_pointer_clear);
    allocated_pointer_clear = allocated_pointer_next;
  }
  isXDBGinitialized = false;
  XDBG_INTERNAL_SUCCESS("XDBG Cleared.", file, line, function);
}

/*********/
/*TESTING*/
/*********/

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
    xdbg_free(f2, __FILE__, __LINE__, __func__); // UB, but we test detection
  }

  // Case 3: free NULL pointer (should be safe)
  printf("Case 3: free NULL pointer (safe)\n");
  int *f3 = NULL;
  xdbg_free(f3, __FILE__, __LINE__, __func__);

  // Case 4: free unallocated pointer (should error)
  printf("Case 4: free unallocated pointer (intentional bad free)\n");
  int dummy;
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
    arr1 = xdbg_realloc(arr1, 20 * sizeof(int), __FILE__, __LINE__, __func__);
    if (!arr1) {
      printf("Case 1 failed: realloc returned NULL\n");
    }
    xdbg_free(arr1, __FILE__, __LINE__, __func__);
  }

  // Case 2: realloc to smaller size
  int *arr2 = xdbg_malloc(20 * sizeof(int), __FILE__, __LINE__, __func__);
  if (arr2) {
    printf("Case 2: realloc to smaller size\n");
    arr2 = xdbg_realloc(arr2, 5 * sizeof(int), __FILE__, __LINE__, __func__);
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
    arr5 = xdbg_realloc(arr5, 20 * sizeof(int), __FILE__, __LINE__, __func__);
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

int main(void) {
  // test_malloc();
  // test_free();
  // test_realloc();
  // test_calloc();

  XDBG_REPORT();
  XDBG_CLEAR();

#ifdef IMD_MEMORY_DEBUG
  xdbg_internal_memory_debug();
#endif // IMD_MEMORY_DEBUG

  XDBG_INTERNAL_WARNING("XDBG TESTS ENDED", __FILE__, __LINE__, __func__);

  return 0;
}
