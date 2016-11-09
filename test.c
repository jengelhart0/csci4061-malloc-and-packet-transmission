#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mm.h"

void run_test(int chunk_size, int num_chunks) {

    printf("Testing mm functions with chunk_size %d and num_chunks %d\n", chunk_size, num_chunks);

    mm_t test;
    mm_t *testptr = &test;
    mm_init(testptr, num_chunks, chunk_size);

    int i, j;

    printf("Beginning of all memory chunks: \n");
    for(i = 0; i < num_chunks; i++) {
	printf("\t%p\n", testptr->data + (i * chunk_size));
    }
    printf("\n");

    char new_char;
    char *testchunk;    
    int num_trials = num_chunks / 2;
    int num_on_stack;
    for(i = 0; i < num_trials; i++) {

	printf("Current memory segment managed: \n");
	int k;
	for(j = 0; j < num_chunks * chunk_size; j+=chunk_size) {
	    printf("\tChunk %d: ", j / chunk_size);
	    for(k = 0; k < chunk_size; k++) {
                printf("%c", *(testptr->data + k + j));
	    }
	    printf("\n");
	}        
	printf("\n");
	printf("next free: %p mem_stack: %p \n", testptr->next_free, testptr->mem_stack);
	num_on_stack = (testptr->next_free - testptr->mem_stack);
	printf("Num on stack: %d\n", num_on_stack);
        printf("Free chunks on stack: \n");	
        for(j = 0; j < num_on_stack + 1; j++) {
	    printf("\t%p\n", *((testptr->mem_stack) + j));
	}
	printf("\n");

        printf("Chunk pointer at next free: %p\n", *(testptr->next_free));
	testchunk = mm_get(testptr);
	printf("Current chunk from get(): %p\n", testchunk); 
	new_char = rand() % 128;
	printf("Testchunk %ld being set to ", (testchunk - testptr->data) / chunk_size); 
	for(j = 0; j < chunk_size; j++) {
	    *(testchunk + j) = new_char;
	    printf("%c", *(testchunk + j));
	}
	printf("\n");
    }		    

//    for(i = 0; i < num_trials; i++)
//        mm_put(testptr, testchunk);

}

void run_tests() {

      run_test(4, 12);
//    run_test(9, 36);
//    run_test(64, 1000);
//    run_test(1, 27);
//    run_test(0, 12);
//    run_test(2, 123);

}

int main() {
    run_tests();
}
