#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mm.h"

/* 
 * Timer for mm_t memory manager
 */

void main() {

  struct timeval time_s, time_e;
  mm_t *testptr = malloc(sizeof(mm_t));
  mm_init(testptr, 1000000, 64);
  int i;
  char *ptrs[1000000];

  /* start timer */
  gettimeofday(&time_s, NULL);
  
  // adding to array required for malloc, controlling differences by doing it here
  for(i = 0; i < 1000000; i++) {
      ptrs[i] = (char *) mm_get(testptr);
 
  }
  for(i = 0; i < 1000000; i++) {
      mm_put(testptr, ptrs[i]);
  }

  gettimeofday(&time_e, NULL);

  fprintf(stderr, "Time taken by mm_t = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
    
}
