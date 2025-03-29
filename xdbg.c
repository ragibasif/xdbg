#include "xdbg.h"

/************************************************************************/
/*INTERNAL DEBUGGING*/
/************************************************************************/

#define INTERNAL_MEMORY_DEBUG
#include "internal_memory_debug.h"

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

enum memory_function {
  MEM_FUNC_MALLOC,
  MEM_FUNC_REALLOC,
  MEM_FUNC_CALLOC,
  MEM_FUNC_FREE
};

struct xdbg_allocated_pointer {
  void *pointer;                 // address of the pointer
  size_t size;                   // size of the allocated memory in bytes
  enum memory_function mem_func; // type of memory function
  const char *file;              // name of file the pointer was allocated/freed
  unsigned int line;             // line number of the allocation/free
  const char *function;          // function name of the allocation/free
  unsigned int total_allocs;     // total number of allocations so far
  unsigned int total_frees;      // total number of frees so far
  struct xdbg_allocated_pointer *next; // the address of the next struct
};

struct xdbg_allocated_pointer *allocated_pointer_head;
struct xdbg_allocated_pointer *allocated_pointer_tail;

static inline void
xdbg_internal_pointer_print_format(struct xdbg_allocated_pointer *pointer) {
  char *memory_function_literal;
  switch (pointer->mem_func) {
  case MEM_FUNC_MALLOC:
    memory_function_literal = "malloc\0";
    break;
  case MEM_FUNC_CALLOC:
    memory_function_literal = "calloc\0";
    break;
  case MEM_FUNC_REALLOC:
    memory_function_literal = "realloc\0";
    break;
  case MEM_FUNC_FREE:
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
  printf("[Total Allocations] %s%s%u%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         pointer->total_allocs, XDBG_ANSI_RESET);
  xdbg_print_prefix();
  printf("[Total Frees] %s%s%u%s\n", XDBG_ANSI_BLUE, XDBG_ANSI_BOLD,
         pointer->total_frees, XDBG_ANSI_RESET);
}

void *xdbg_malloc(size_t size, const char *file, unsigned int line,
                  const char *function) {
  xdbg_init_check(file, line, function);

  void *pointer = (void *)malloc(size);
  struct xdbg_allocated_pointer *curr = (struct xdbg_allocated_pointer *)malloc(
      sizeof(struct xdbg_allocated_pointer));
  xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);

  curr->pointer = pointer;
  curr->size = size;
  curr->mem_func = MEM_FUNC_MALLOC;
  curr->file = file;
  curr->line = line;
  curr->function = function;
  curr->next = NULL;

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

void xdbg_free(void *pointer, const char *file, unsigned int line,
               const char *function) {
  xdbg_init_check(file, line, function);
  struct xdbg_allocated_pointer *curr = (struct xdbg_allocated_pointer *)malloc(
      sizeof(struct xdbg_allocated_pointer));
  xdbg_internal_alloc_check(curr, __FILE__, __LINE__, __func__);
  curr->pointer = pointer;
  curr->size = 0;
  curr->mem_func = MEM_FUNC_FREE;
  curr->file = file;
  curr->line = line;
  curr->function = function;
  curr->next = NULL;

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

/*for testing during development*/
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
  int *p = xdbg_malloc(300, __FILE__, __LINE__, __func__);
  int *q = xdbg_malloc(300 * sizeof(int), __FILE__, __LINE__, __func__);
  int *w = xdbg_malloc(300, __FILE__, __LINE__, __func__);
  int *s = xdbg_malloc(300, __FILE__, __LINE__, __func__);
  /*int *p = malloc(300);*/
  xdbg_free(p, __FILE__, __LINE__, __func__);

  XDBG_REPORT();
  XDBG_CLEAR();

#ifdef INTERNAL_MEMORY_DEBUG
  xdbg_internal_memory_debug();
#endif // INTERNAL_MEMORY_DEBUG

  XDBG_INTERNAL_WARNING("XDBG TESTS ENDED", __FILE__, __LINE__, __func__);

  return 0;
}
