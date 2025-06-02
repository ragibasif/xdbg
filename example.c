#define XDBG_ENABLE
#include "xdbg.h"

int main(void) {
    XDBG_INITIALIZE();
    int *a = malloc(100 * sizeof(*a));
    free(a);
    XDBG_REPORT();
    XDBG_FINALIZE();
    return 0;
}
