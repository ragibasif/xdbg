#include "xdbg.h"

// Summary of Malloc Test Cases:
// Case     Description
// 1	Normal allocation
// 2	Allocation with size = 0
// 3	Huge allocation (OOM)
// 4	Leak detection (intentional no free)
void test_malloc(void) {
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
