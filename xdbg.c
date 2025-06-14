// xdbg.c

#define F_MEMORY_DEBUG /* turns on the memory debugging system */

#include "xdbg.h"

// #define DEFAULT_BUFFER 1024

void x_create(void) {
    xdbg = malloc(sizeof(*xdbg));
    if (!xdbg) {
        fprintf(stderr, "Memory allocation error!\n");
        return;
    }
    data_create();
    report_create();
}

void x_destroy(void) {
    data_destroy();
    report_destroy();
    free(xdbg);
    xdbg = NULL;
}

void report_create(void) {
    xdbg->report = malloc(sizeof(*xdbg->report));
    if (!xdbg->report) {
        fprintf(stderr, "Memory allocation error!\n");
        return;
    }
    xdbg->report->allocation_count = 0;
    xdbg->report->free_count = 0;
    xdbg->report->allocated_bytes = 0;
    xdbg->report->freed_bytes = 0;
}

void report_destroy(void) {
    xdbg->report->allocation_count = 0;
    xdbg->report->free_count = 0;
    xdbg->report->allocated_bytes = 0;
    xdbg->report->freed_bytes = 0;
    free(xdbg->report);
    xdbg->report = NULL;
}

void data_create(void) {
    xdbg->data = malloc(sizeof(*xdbg->data));
    if (!xdbg->data) {
        fprintf(stderr, "Memory allocation error!\n");
        return;
    }
    xdbg->data->head = NULL;
    xdbg->data->tail = NULL;
}

void data_destroy(void) {
    // TODO: free head
    Allocator *prev;
    Allocator *curr;

    // FIX: refact to use allocator_destroy()
    prev = NULL;
    curr = xdbg->data->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
        prev = NULL;
    }

    // TODO: free tail
    free(xdbg->data);
    xdbg->data = NULL;
}
