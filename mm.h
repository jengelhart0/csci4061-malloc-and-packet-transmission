#ifndef __MM_H
#define __MM_H

#include <sys/time.h>

#define INTERVAL 0
#define INTERVAL_USEC 50000
#define CHUNK_SIZE 64
#define NUM_CHUNKS 1000000

typedef enum command {PUT, GET};

/* Fill this in */
typedef struct {
    int chunk_size;
    int num_chunks;
    char *data;
    char **mem_stack;
    char **next_free;
} mm_t;

/* Implement these in mm.c */
double comp_time(struct timeval time_s, struct timeval time_e);
static int is_already_free(mm_t *mm, char *chunk, enum command porg);
int mm_init(mm_t *mm, int num_chunks, int chunk_size);
void *mm_get(mm_t *mm);
void mm_put(mm_t *mm, void *chunk);
void mm_release(mm_t *mm);

#endif
