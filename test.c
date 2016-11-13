#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mm.h"

void print_stack(mm_t *testptr) {

    int num_on_stack;

    printf("\n");
    printf("next free: %p mem_stack: %p \n", testptr->next_free, testptr->mem_stack);
    num_on_stack = (testptr->next_free - testptr->mem_stack) + 1;
    printf("Num on stack: %d\n", num_on_stack);
    printf("Free chunks on stack: \n");	
    char **msptr;
    for(msptr = testptr->mem_stack; msptr <= testptr->next_free; msptr++) {
        printf("\t%p\n", *msptr);
    }
    printf("\n");
    printf("Chunk pointer at next free: %p\n", *(testptr->next_free));

}

void print_memory(mm_t *testptr, int num_chunks, int chunk_size) {

    printf("Current memory segment managed: \n");
    int j, k;
    for(j = 0; j < num_chunks * chunk_size; j+=chunk_size) {
        printf("\tChunk %d: ", j / chunk_size);
        for(k = 0; k < chunk_size; k++) {
            printf("%c", *(testptr->data + k + j));
        }
        printf("\n"); 
    } 
    
}

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
    int num_trials = num_chunks;
    char *putchunks[num_chunks];

    // test mm_get
    for(i = 0; i < num_trials; i++) {

	print_stack(testptr);       

        testchunk = mm_get(testptr);
	putchunks[i] = testchunk;

	printf("Current chunk from get(): %p\n", testchunk); 
	new_char = rand() % 93 + 33;
	if (new_char == 32) new_char++;
	printf("Testchunk %ld being set to ", (testchunk - testptr->data) / chunk_size); 
	for(j = 0; j < chunk_size; j++) {
	    *(testchunk + j) = new_char;
	    printf("%c", *(testchunk + j));
	}
	printf("\n");

	print_memory(testptr, num_chunks, chunk_size);       

    }		    

    // test mm_put
    for(i = 0; i < num_trials; i++) {
	printf("\nPutting chunk %d, address %p back on mem_stack\n", i, putchunks[i]);
        mm_put(testptr, putchunks[i]);
	print_stack(testptr);       
	print_memory(testptr, num_chunks, chunk_size);       

    }

    // retest mm_put to check for re-put handling
    for(i = 0; i < 4; i++) {
	printf("\nPutting chunk %d, address %p back on mem_stack\n", i, putchunks[i]);
        mm_put(testptr, putchunks[i]);    
    }

    // retest mm_get when all blocks should be free again

    for(i = 0; i < num_trials; i++) {

	print_stack(testptr);       

        testchunk = mm_get(testptr);
	putchunks[i] = testchunk;

	printf("Current chunk from get(): %p\n", testchunk); 
	new_char = rand() % 93 + 33;
	if (new_char == 32) new_char++;
	printf("Testchunk %ld being set to ", (testchunk - testptr->data) / chunk_size); 
	for(j = 0; j < chunk_size; j++) {
	    *(testchunk + j) = new_char;
	    printf("%c", *(testchunk + j));
	}
	printf("\n");
	print_memory(testptr, num_chunks, chunk_size);       

    }	

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
