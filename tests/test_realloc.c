#include "../xdbg.h"

// Summary of Realloc Test Cases:
// Case     Description
// 1        Realloc to a larger size
// 2        Realloc to a smaller size
// 3        Realloc from NULL (acts like malloc)
// 4        Realloc to size 0 (acts like free)
// 5        Realloc after free (UB, but test if it’s handled)
// 6        Realloc to extremely large size to simulate OOM
void test_realloc(void) {
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
    } else {
        xdbg_free(arr3, __FILE__, __LINE__, __func__);
    }

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
