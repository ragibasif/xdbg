#define XDBG_ENABLE
#include "xdbg.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    XDBG_INITIALIZE();
    int *a = malloc(100 * sizeof(*a));
    a[0] = 100;
    free(a);
    XDBG_REPORT();
    XDBG_FINALIZE();
    return 0;
}
