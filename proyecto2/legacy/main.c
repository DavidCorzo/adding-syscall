#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "thpool.h"
#include "stats.h"

int main() {
    threadpool thpool = thpool_init(4);
    int i = 998;
    do {
        thpool_add_work(thpool, single_paralel_file_model, (void*)(uintptr_t)i);
    } while (i--);
    thpool_wait(thpool);
    thpool_destroy(thpool);
    return 0;
}