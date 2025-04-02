#include "xdbg.h"

/************************************************************************/
/*INTERNAL DEBUGGING*/
/************************************************************************/

#define INTERNAL_MEMORY_DEBUG
#include "imd.h"

#ifdef INTERNAL_MEMORY_DEBUG
static void xdbg_internal_memory_debug(void) {
  debug_memory_init(NULL, NULL, NULL);
  debug_mem_print(0);
  debug_mem_reset();
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

  int *pointer = malloc(number * size);
  xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);
  size_t index;
  for (index = 0; index < number; index++) {
    pointer[index] = 0;
  }
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

  if (!pointer) {
    return xdbg_malloc(size, __FILE__, __LINE__, __func__);
  } else if (size == 0) {
    xdbg_free(pointer, __FILE__, __LINE__, __func__);
    return NULL;
  }

  struct xdbg_allocated_pointer *curr = malloc(sizeof(*curr));
  xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

  curr->pointer = xdbg_malloc(size, __FILE__, __LINE__, __func__);
  curr->size = size;
  memcpy(curr->pointer, pointer, curr->size);
  curr->memory_function = MEMORY_FUNCTION_REALLOC;
  curr->file = file;
  curr->line = line;
  curr->function = function;
  curr->next = NULL;

  curr->freed = false;

  free(pointer);
  pointer = xdbg_malloc(size, __FILE__, __LINE__, __func__);
  xdbg_internal_alloc_check(pointer, __FILE__, __LINE__, __func__);
  memcpy(pointer, curr->pointer, curr->size);

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

static void *xdbg_find_pointer(void *pointer) {
  struct xdbg_allocated_pointer *curr = allocated_pointer_head;
  while (curr) {
    if (curr->pointer == pointer) {
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

void xdbg_free(void *pointer, const char *file, unsigned int line,
               const char *function) {
  xdbg_init_check(file, line, function);
  if (pointer == NULL) {
    XDBG_INTERNAL_ERROR("Attempted to free a NULL pointer.", file, line,
                        function);
    exit(EXIT_FAILURE);
  }
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
  if (allocated_pointer_head == NULL) {
    allocated_pointer_head = curr;
    allocated_pointer_tail = allocated_pointer_head;
  } else {
    allocated_pointer_tail->next = curr;
    allocated_pointer_tail = allocated_pointer_tail->next;
  }
  if (pointer != NULL) {
    free(pointer);
    pointer = NULL;
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

static void test0(void) {
  int *p = xdbg_malloc(2, __FILE__, __LINE__, __func__);
  int *q = xdbg_malloc(300 * sizeof(*q), __FILE__, __LINE__, __func__);
  int *w = xdbg_malloc(197, __FILE__, __LINE__, __func__);
  int *s = xdbg_malloc(10239, __FILE__, __LINE__, __func__);
  xdbg_free(p, __FILE__, __LINE__, __func__);
  p = NULL;
}

static void test1(void) {
  int *p = xdbg_malloc(473, __FILE__, __LINE__, __func__);
  xdbg_free(p, __FILE__, __LINE__, __func__);
  p = NULL;
  int *q = xdbg_malloc(338 * sizeof(int), __FILE__, __LINE__, __func__);
  xdbg_free(q, __FILE__, __LINE__, __func__);
  q = NULL;
  int *w = xdbg_malloc(3, __FILE__, __LINE__, __func__);
  int *s = xdbg_malloc(80000, __FILE__, __LINE__, __func__);
  xdbg_free(w, __FILE__, __LINE__, __func__);
  xdbg_free(s, __FILE__, __LINE__, __func__);
  w = NULL;
  s = NULL;
}

static void test_calloc(void) {
  int *arr = xdbg_calloc(50, sizeof(int), __FILE__, __LINE__, __func__);
  if (arr) {
    for (int i = 0; i < 50; i++) {
      if (arr[i] != 0) {
        printf("calloc failed to initialize memory to zero\n");
      }
    }
    xdbg_free(arr, __FILE__, __LINE__, __func__);
  }
}

static void test_realloc(void) {
  int *arr = xdbg_malloc(10 * sizeof(*arr), __FILE__, __LINE__, __func__);
  if (arr) {
    arr = xdbg_realloc(arr, 20 * sizeof(int), __FILE__, __LINE__, __func__);
    if (!arr) {
      printf("realloc failed\n");
    }
    xdbg_free(arr, __FILE__, __LINE__, __func__);
  }
}

static void test_double_free(void) {
  int *p = xdbg_malloc(100, __FILE__, __LINE__, __func__);
  xdbg_free(p, __FILE__, __LINE__, __func__);
  xdbg_free(p, __FILE__, __LINE__,
            __func__); // Intentional double free to test xdbg detection
}

static void test_large_alloc(void) {
  int *p = xdbg_malloc(1024 * 1024 * 10, __FILE__, __LINE__,
                       __func__); // 10MB allocation
  if (p) {
    xdbg_free(p, __FILE__, __LINE__, __func__);
  } else {
    printf("Large allocation failed\n");
  }
}

static void test_memory_leak(void) {
  int *p = xdbg_malloc(512, __FILE__, __LINE__, __func__);
  // Intentional memory leak, should be detected by xdbg
}

static void test_invalid_free(void) {
  int *p = xdbg_malloc(100, __FILE__, __LINE__, __func__);
  xdbg_free(p, __FILE__, __LINE__, __func__);
  xdbg_free(p + 1, __FILE__, __LINE__,
            __func__); // Invalid free, testing out-of-bounds detection
}

static void test_null_free(void) {
  int *p = NULL;
  xdbg_free(p, __FILE__, __LINE__,
            __func__); // Should be safe but tested for robustness
}

int main(void) {
  puts("\n .·:''''''''''''''''''''''''''''''''''''':·.\n : :                   "
       "                  : :\n : :  ██╗  ██╗██████╗ ██████╗  ██████╗   : "
       ":\n : :  ╚██╗██╔╝██╔══██╗██╔══██╗██╔════╝   : :\n : :   ╚███╔╝ ██║  "
       "██║██████╔╝██║  ███╗  : :\n : :   ██╔██╗ ██║  ██║██╔══██╗██║   ██║  "
       ": :\n : :  ██╔╝ ██╗██████╔╝██████╔╝╚██████╔╝  : :\n : :  ╚═╝  "
       "╚═╝╚═════╝ ╚═════╝  ╚═════╝   : :\n : :                              "
       "       : :\n '·:.....................................:·'\n ");
  XDBG_INTERNAL_WARNING("XDBG TESTS STARTED", __FILE__, __LINE__, __func__);
  XDBG_INIT();
  test0();
  test1();
  test_realloc();
  test_calloc();
  /*test_double_free();*/
  test_large_alloc();
  test_memory_leak();
  /*test_invalid_free();*/
  /*test_null_free();*/

  XDBG_REPORT();
  XDBG_CLEAR();

#ifdef INTERNAL_MEMORY_DEBUG
  xdbg_internal_memory_debug();
#endif // INTERNAL_MEMORY_DEBUG

  XDBG_INTERNAL_WARNING("XDBG TESTS ENDED", __FILE__, __LINE__, __func__);

  return 0;
}
