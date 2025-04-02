#define XDBG_ENABLE
#include "xdbg.h"
#include <stdio.h>
#include <stdlib.h>

void test0(void) {
  int *p = malloc(2);
  int *q = malloc(300 * sizeof(*q));
  int *w = malloc(197);
  int *s = malloc(10239);
  free(p);
  p = NULL;
}

void test1(void) {
  int *p = malloc(473);
  free(p);
  p = NULL;
  int *q = malloc(338 * sizeof(*q));
  free(q);
  q = NULL;
  int *w = malloc(3);
  int *s = malloc(80000);
  free(w);
  free(s);
  w = NULL;
  s = NULL;
}

void test_calloc(void) {
  int *arr = calloc(50, sizeof(*arr));
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
  int *arr = malloc(10 * sizeof(*arr));
  if (arr) {
    arr = realloc(arr, 20 * sizeof(*arr));
    if (!arr) {
      printf("realloc failed\n");
    }
    free(arr);
  }
}

void test_double_free(void) {
  int *p = malloc(100);
  free(p);
  free(p); // Intentional double free to test xdbg detection
}

void test_large_alloc(void) {
  int *p = malloc(1024 * 1024 * 10); // 10MB allocation
  if (p) {
    free(p);
  } else {
    printf("Large allocation failed\n");
  }
}

void test_memory_leak(void) {
  int *p = malloc(512);
  // Intentional memory leak, should be detected by xdbg
}

void test_invalid_free(void) {
  int *p = malloc(100);
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
