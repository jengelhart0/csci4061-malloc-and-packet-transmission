#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mm.h"
/* 
 * Timer for mm_t memory manager
 */

void main() {

  struct timeval time_s, time_e;
  int i;
  char *ptrs[1000000];

  /* start timer */
  gettimeofday(&time_s, NULL);
  
  for(i = 0; i < 1000000; i++) {
      ptrs[i] = malloc(sizeof(char) * 64);
  }
  for(i = 0; i < 1000000; i++) {
      free(ptrs[i]);
  }

  gettimeofday(&time_e, NULL);

  fprintf(stderr, "Time taken by malloc = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
    
}
