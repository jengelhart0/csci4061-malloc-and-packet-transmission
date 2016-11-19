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

/*
 * This function is used to protect against duplicate mm_put() calls
 * Though O(1), it damages performance against malloc(), so calls to
 * it are commented out for the purposes of grading/submission.
 */
static int is_already_free(mm_t *mm, char *chunk, enum command porg) {

  int chunk_idx, which_int, which_pos, bits_in_int, chunk_mask;
  
  bits_in_int = (sizeof(int) * 8);
  // use distance from data start to chunk and chunk_size to find which chunk it is in sequence
  chunk_idx = (chunk - mm->data) / mm->chunk_size;
  // use chunk_idx and bits_in_int to find which int in already_free ints we should examine
  which_int = chunk_idx / bits_in_int;
  // find which bit offset into which_int represents our chunk
  which_pos = chunk_idx % bits_in_int;
  chunk_mask = 1 << ((bits_in_int - 1) - which_pos);

  if(porg == PUT) {
    // if bit for this chunk is already_free (i.e., it == 1), return 1
    if(chunk_mask & *((mm->already_free) + which_int)) {
      return 1;
    } else {
      // otherwise sit that bit to 1 and return 0
      *((mm->already_free) + which_int) |= chunk_mask;
      return 0;
    }
  } else if(porg == GET) {
    *((mm->already_free) + which_int) ^= chunk_mask;
    return 0;
  }
  return -1;
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

/* The following is part of is_already_free() implementation */
//// get num_ints needed to have one bit to represent each chunk
//  int num_ints = (hm / ((float) sizeof(int) * 8)) + 1;
//  if((mm->already_free = malloc(num_ints * sizeof(int))) == NULL) {
//     perror("Failed to allocate memory for already_free\n");
//     return -1;
//  }
//// by setting all bits in int array to 1, it indicates all chunks start free
//  for(i = 0; i < num_ints; i++) {
//    *((mm->already_free) + i) = -1;
//  }

  return 0;  /* return the right value */
}

/* Implement */
void *mm_get(mm_t *mm) {

  // last legal chunk address is chunk_size less than total # of bytes
  if(mm->next_free >= mm->mem_stack) { 
    void* requested; 
    requested = *(mm->next_free);

/* The following is part of is_already_free() implementation */
//    if(is_already_free(mm, (char *) requested, GET) == -1) {
//      perror("Illegal command passed to is_already_free: exiting\n");
//      exit(0);
//    }					    
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

/* The following is part of is_already_free() implementation */
//  else if((is_free = is_already_free(mm, casted_chunk, PUT)) == -1) {
//    fprintf(stderr, "Illegal command passed to is_already_free()\n");
//  }

//  else if(is_free) {
//    fprintf(stderr, "Chunk is already free!\n");
//  }

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
//  free(mm->already_free);
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
