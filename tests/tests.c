#define XDBG_ENABLE
#include "xdbg.h"
#include <stdio.h>
#include <stdlib.h>

void test0(void) {
  int *p = (int *)malloc(2);
  int *q = (int *)malloc(300 * sizeof(int));
  int *w = (int *)malloc(197);
  int *s = (int *)malloc(10239);
  free(p);
  p = NULL;
}

void test1(void) {
  int *p = (int *)malloc(473);
  free(p);
  p = NULL;
  int *q = (int *)malloc(338 * sizeof(int));
  free(q);
  q = NULL;
  int *w = (int *)malloc(3);
  int *s = (int *)malloc(80000);
  free(w);
  free(s);
  w = NULL;
  s = NULL;
}

void test_calloc(void) {
  int *arr = (int *)calloc(50, sizeof(int));
  if (arr) {
    for (int i = 0; i < 50; i++) {
      if (arr[i] != 0) {
        printf("calloc failed to initialize memory to zero\n");
      }
    }
    free(arr);
  }
}

void test_realloc(void) {
  int *arr = (int *)malloc(10 * sizeof(int));
  if (arr) {
    arr = (int *)realloc(arr, 20 * sizeof(int));
    if (!arr) {
      printf("realloc failed\n");
    }
    free(arr);
  }
}

void test_double_free(void) {
  int *p = (int *)malloc(100);
  free(p);
  free(p); // Intentional double free to test xdbg detection
}

void test_large_alloc(void) {
  int *p = (int *)malloc(1024 * 1024 * 10); // 10MB allocation
  if (p) {
    free(p);
  } else {
    printf("Large allocation failed\n");
  }
}

void test_memory_leak(void) {
  int *p = (int *)malloc(512);
  // Intentional memory leak, should be detected by xdbg
}

void test_invalid_free(void) {
  int *p = (int *)malloc(100);
  free(p);
  free(p + 1); // Invalid free, testing out-of-bounds detection
}

void test_null_free(void) {
  int *p = NULL;
  free(p); // Should be safe but tested for robustness
}

int main(void) {
  XDBG_INIT();
  test0();
  test1();
  test_calloc();
  test_realloc();
  test_double_free();
  test_large_alloc();
  test_memory_leak();
  test_invalid_free();
  test_null_free();
  XDBG_REPORT();
  XDBG_CLEAR();

  return 0;
}
