#include "xdbg.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    x_create();
    x_destroy();

    f_debug_memory_init(
        NULL, NULL, NULL); /* Required for memory debugger to be thread safe */
    f_debug_mem_print(0);  /* Prints§ out a list of all allocations made, their
                     location, how much memorey each has allocated, freed,
                     and how many allocations have been made. The min_allocs
                     parameter can be set to avoid printing any allocations
                     that have been made fewer times then min_allocs */
    dbg(f_debug_mem_consumption()); /* add up all memory consumed by mallocsd
                                      and reallocs covered by the memory
                                      debugger .*/
    dbg(f_debug_memory()); /*f_debug_memory checks if any of the bounds of any
                         allocation has been over written and reports where to
                         standard out. The function returns TRUE if any error
                         was found*/
    f_debug_mem_reset();   /* f_debug_mem_reset allows you to clear all memory
                    stored   in the debugging system if you only want to record
                    allocations   after a specific point in your code*/

    return 0;
}
