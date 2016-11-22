#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mm.h"

/* Return usec */
double comp_time(struct timeval time_s, struct timeval time_e) {

  double elap = 0.0;

  if (time_e.tv_sec > time_s.tv_sec) {
    elap += (time_e.tv_sec - time_s.tv_sec - 1) * 1000000.0;
    elap += time_e.tv_usec + (1000000 - time_s.tv_usec);
  }
  else {
    elap = time_e.tv_usec - time_s.tv_usec;
  }
  return elap;
}

/* Implement.  Return 0 for success, or -1 and set errno on fail. */
int mm_init(mm_t *mm, int hm, int sz) {

  mm->num_chunks = hm;
  mm->chunk_size = sz;

  // size allocated must be atleast 1
  if(mm->chunk_size < 1){
    fprintf(stderr, "Not a valid memory manager size: %d.\n", mm->chunk_size);
  }

  if((mm->mem_stack = (char **) malloc(sizeof(char *) * hm)) == NULL) {
    perror("Failed to allocate memory for memory stack\n");
    return -1;
  }  

  if((mm->data = (char *) malloc(sizeof(char) * hm * sz)) == NULL) {
    perror("Failed to allocate memory for memory manager\n");
    return -1;
  }

  int i;
  for(i = 0; i < hm; i++) {
    *((mm->mem_stack) + i) = (mm->data) + (i * sz);
  }
  mm->next_free = (mm->mem_stack) + (i - 1);  

  return 0;  /* return the right value */
}

/* Implement */
void *mm_get(mm_t *mm) {

  // last legal chunk address is chunk_size less than total # of bytes
  if(mm->next_free >= mm->mem_stack) { 
    void* requested; 
    requested = *(mm->next_free);
				    
    mm->next_free -= 1;
    return requested;

  } else {
    fprintf(stderr, "No free memory available.\n");
    return NULL;
  }
}

/* Implement */
void mm_put(mm_t *mm, void *chunk) {

  char *casted_chunk = (char *) chunk; 
  int is_free;

  if(casted_chunk < mm->data || casted_chunk > mm->data + (mm->chunk_size * (mm->num_chunks - 1))) {
    fprintf(stderr, "Chunk %p outside managed memory.\n", casted_chunk);
  } 

  // casted_chunk must point to the beginning of a valid casted_chunk
  else if((casted_chunk - mm->data) % mm->chunk_size) {
    fprintf(stderr, "Chunk %p does not reference beginning of a valid memory segment\n", casted_chunk);
  }

  // push chunk onto mem_stack 
  else {
    (mm->next_free) += 1;
    *(mm->next_free) = chunk;    
  }
}

/* Implement */
void mm_release(mm_t *mm) {
  free(mm->data);
  free(mm->mem_stack);
}

/*
 * This is just an example of how to use the timer.  Notice that
 * this function is not included in mm_public.h, and it is defined as static,
 * so you cannot call it from other files.  Instead, just follow this model
 * and implement your own timing code where you need it.
 */
static void timer_example() {
  struct timeval time_s, time_e;

  /* start timer */
  gettimeofday (&time_s, NULL);

  /* code you wish to time goes here */

  gettimeofday(&time_e, NULL);

  fprintf(stderr, "Time taken = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
}
