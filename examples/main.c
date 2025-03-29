#define XDBG_ENABLE
#include "xdbg.h"

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
int main(void) {
  xdbg_init();
  test0();
  test1();
  xdbg_report();
  xdbg_clear();

  return 0;
}
