// example.c
#define XDBG_ENABLE
#include "xdbg.h"

#define MAX_BUFFER_SIZE 100

void malloc_example(void) {
    // normal allocation
    int *a1 = malloc(MAX_BUFFER_SIZE * sizeof(*a1));
    size_t i;
    for (i = 0; i < MAX_BUFFER_SIZE; i++) {
        a1[i] = i << 2;
    }
    free(a1);

    //  malloc 0 bytes
    int *a2 = malloc(0);
    free(a2);

    // malloc and forget to free (leak detection test)
    void *a4 = malloc(64);
    (void)a4; // intentionally not freeing
}

int main(void) {
    XDBG_INITIALIZE();
    malloc_example();
    XDBG_REPORT();
    XDBG_FINALIZE();
    return 0;
}
