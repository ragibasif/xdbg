#include "../xdbg.h"

// Summary of Calloc Test Cases:
// Case     Description
// 1	Normal zero-initialized memory
// 2	calloc(0, size)
// 3	calloc(n, 0)
// 4	Overflow detection
extern void test_calloc(void) {
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
