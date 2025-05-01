#include "xdbg.h"

// Summary of Free Test Cases:
// Case     Description
// 1	Normal free
// 2	Double free
// 3	Free NULL
// 4	Free invalid pointer
void test_free(void) {
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
