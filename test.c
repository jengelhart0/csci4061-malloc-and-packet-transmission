#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mm.h"

void run_test(int chunk_size, int num_chunks) {

    printf("Testing mm functions with chunk_size %d and num_chunks %d\n", chunk_size, num_chunks);

    mm_t test;
    mm_t *testptr = &test;
    mm_init(testptr, num_chunks, chunk_size);

    void *mem_chunk[num_chunks];
    char pack[chunk_size];
    int i, j;
    for(i = 0; i < testptr->num_chunks; i++) {
	mem_chunk[i] = mm_get(testptr);

        for(j = 0; j < chunk_size; j++) {
	    pack[j] = 64 + ('a' + i) % 52;    
 	}

        memcpy(mem_chunk[i], pack, chunk_size);
    }

    for(i = 0; i < testptr->num_chunks * testptr->chunk_size; i++) {
      printf("%c", *((testptr->data)+i));
    }
    printf("\n");

    printf("Testing put function...\n");
    int idx;
    char *old_free, *new_free;
    for(i = 0; i < testptr->num_chunks; i++) {
	idx = rand() % num_chunks; 
        char *puttestptr = mem_chunk[idx];
        char *lastusedptr = (testptr->next_free) - testptr->chunk_size;


        printf("last used: ");
        for(j = 0; j < testptr->chunk_size; j++) {
	    printf("%c", *(lastusedptr + j));
	    
	}
	printf("\n");

        old_free = testptr->next_free;
        printf("at put chunk pre-put: ");
        for(j = 0; j < testptr->chunk_size; j++) {
        	    printf("%c", *(puttestptr + j));
	}
        printf("\n");
        mm_put(testptr, puttestptr);

        printf("at put chunk post-put: ");
        for(j = 0; j < testptr->chunk_size; j++) {    
	    printf("%c", *(puttestptr + j));
	}        
	printf("\n");
        new_free = testptr->next_free;
	if(old_free - (new_free + chunk_size)) {
	    printf("New next_free NOT equal to old next_free - chunk_size: old: %p new: %p\n", old_free, new_free);
	}
	else {
	    printf("New next_free equal to old next_free - chunk_size: old: %p new: %p\n", old_free, new_free);
	}	
	printf("\n\n");

    }
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
