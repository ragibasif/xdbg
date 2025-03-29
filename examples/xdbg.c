#include "xdbg.h"

#define XDBG_PREFIX                                                            \
  printf("(%s%s%s%s) ", XDBG_ANSI_MAGENTA, XDBG_ANSI_BOLD, "xdbg",             \
         XDBG_ANSI_RESET)
#define XDBG_ARROW_PREFIX                                                      \
  do {                                                                         \
    XDBG_PREFIX;                                                               \
    printf("%s%s%s%s ", XDBG_ANSI_MAGENTA, XDBG_ANSI_BOLD, ":>",               \
           XDBG_ANSI_RESET);                                                   \
  } while (0);
#define XDBG_INTERNAL_ERROR(msg)                                               \
  xdbg_internal_log_wrapper(msg, "ERROR\0", XDBG_ANSI_RED)
#define XDBG_INTERNAL_SUCCESS(msg)                                             \
  xdbg_internal_log_wrapper(msg, "SUCCESS\0", XDBG_ANSI_GREEN)
#define XDBG_INTERNAL_WARNING(msg)                                             \
  xdbg_internal_log_wrapper(msg, "WARNING\0", XDBG_ANSI_YELLOW)

static inline void xdbg_internal_log_wrapper(const char *message,
                                             const char *prefix,
                                             const char *color) {

  XDBG_PREFIX;
  printf("[%s%s%s%s] %s\n", color, XDBG_ANSI_BOLD, prefix, XDBG_ANSI_RESET,
         message);
}

static void xdbg_internal_alloc_check(void *pointer, const char *file,
                                      const unsigned int line,
                                      const char *function) {
  if (!pointer) {
    XDBG_INTERNAL_ERROR("Memory allocation failed!");
    exit(EXIT_FAILURE);
  }
}

static bool isXDBGinitialized = false;

static inline void xdbg_init_check(const char *file, const unsigned int line,
                                   const char *function) {
  if (!isXDBGinitialized) {
    XDBG_INTERNAL_ERROR("Must call xdbg_init() first.");
    exit(EXIT_FAILURE);
  }
}

struct xdbg_allocation_record {
  unsigned int allocated;
  unsigned int freed;
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
  unsigned int allocs;           // total number of allocations so far
  unsigned int frees;            // total number of frees so far
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
  XDBG_ARROW_PREFIX;
  printf("Pointer address: %p\n", (void *)pointer->pointer);
  XDBG_ARROW_PREFIX;
  printf("Size of memory in bytes: %lu\n", pointer->size);
  XDBG_ARROW_PREFIX;
  printf("Memory function type: %s\n", memory_function_literal);
  XDBG_ARROW_PREFIX;
  printf("In file: %s\n", pointer->file);
  XDBG_ARROW_PREFIX;
  printf("On line: %u\n", pointer->line);
  XDBG_ARROW_PREFIX;
  printf("Within function: %s\n", pointer->function);
  XDBG_ARROW_PREFIX;
  printf("Allocations so far: %u\n", pointer->allocs);
  XDBG_ARROW_PREFIX;
  printf("Frees so far: %u\n", pointer->frees);
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
  allocation_record.allocated++;
  curr->allocs = allocation_record.allocated;
  curr->frees = allocation_record.freed;
  return pointer;
}

void xdbg_free(void *pointer, const char *file, unsigned int line,
               const char *function) {
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
  allocation_record.freed++;
  curr->allocs = allocation_record.allocated;
  curr->frees = allocation_record.freed;
}

void xdbg_report(void) {
  putchar('\n');
  XDBG_PREFIX;
  puts("XDBG Report Start");
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
  XDBG_ARROW_PREFIX;
  printf("Total Allocated: %u\n", allocation_record.allocated);
  XDBG_ARROW_PREFIX;
  printf("Total Freed: %u\n", allocation_record.freed);
  putchar('\n');
  XDBG_PREFIX;
  puts("XDBG Report End");
  putchar('\n');
}

void xdbg_init(void) {
  XDBG_INTERNAL_WARNING("Initializing XDBG");
  allocated_pointer_head = NULL;
  allocated_pointer_tail = NULL;
  isXDBGinitialized = true;
  XDBG_INTERNAL_SUCCESS("XDBG Initialized");
}

void xdbg_clear(void) {
  XDBG_INTERNAL_WARNING("Clearing XDBG");
  struct xdbg_allocated_pointer *allocated_pointer_clear =
      allocated_pointer_head;
  while (allocated_pointer_clear != NULL) {
    struct xdbg_allocated_pointer *allocated_pointer_next =
        allocated_pointer_clear->next;
    free(allocated_pointer_clear);
    allocated_pointer_clear = allocated_pointer_next;
  }
  isXDBGinitialized = false;
  XDBG_INTERNAL_SUCCESS("XDBG Cleared");
}
