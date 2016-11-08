#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mm.h"

void run_test(int chunk_size, int num_chunks) {

    printf("Testing mm functions with chunk_size %d and num_chunks %d\n", chunk_size, num_chunks);

    mm_t test;
    mm_t *testptr = &test;
    mm_init(testptr, num_chunks, chunk_size);
    void* testchunk = mm_get(testptr);
    mm_put(testptr, testchunk);

}

void run_tests() {

//    run_test(9, 32);
    run_test(64, 1000);
//    run_test(1, 27);
//    run_test(0, 12);
//    run_test(2, 123);

}

int main() {
    run_tests();
}
