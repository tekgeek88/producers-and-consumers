/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2019
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"


// Define Locks and Condition variables here
int use_ptr   = 0;
int fill_ptr  = 0;

static volatile counters_t counters;

// Producer consumer data structures
sem_t empty;
sem_t full;
sem_t mutex;

// Bounded buffer bigmatrix defined in prodcons.h
Matrix ** bigmatrix;

void init_semiphores(int buffer_max) {
  sem_init(&empty, 0, buffer_max);
  sem_init(&full, 0, 0);
  sem_init(&mutex, 0, 1);
}

// Bounded buffer put() get()
int put(Matrix *value, void *args) {
  thread_args_t *params = (thread_args_t*) args;
  if (params->counters->prod->value < BOUNDED_BUFFER_SIZE) {
    bigmatrix[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % MAX;
  }
}

Matrix *get() {
  Matrix *tmp_matrix = bigmatrix[use_ptr];
  use_ptr = (use_ptr + 1) % MAX;
  return tmp_matrix;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg) {
  printf("prod_worker is running!\n");
  thread_args_t *params = (thread_args_t*) arg;

  int loops = NUMBER_OF_MATRICES;
  int i;
#if OUTPUT
  printf("In worker...\n");
  printf("loops=%d\n",*loops);
#endif
  for (i = 0; i < loops; i++) {
    sem_wait(&empty);
    sem_wait(&mutex);
    printf("Creating matrix #%d\n", i);
    Matrix *matrix = GenMatrixRandom();
    DisplayMatrix(matrix, stdout);
    put(matrix, params);
    sem_post(&mutex);
    sem_post(&full);
  }

  // end case
  for (i = 0; i < loops; i++) {
    sem_wait(&empty);
    sem_wait(&mutex);
//    put(-1);
    printf("End case ran!\n");
    sem_post(&mutex);
    sem_post(&full);
  }
  return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg) {
  thread_args_t *params = (thread_args_t*) arg;

  int loops = NUMBER_OF_MATRICES;
  int i;
  for (i = 0; i < loops; i++) {
    sem_wait(&full);
    sem_wait(&mutex);
    Matrix *tmp_matrix = get();
    printf("Consuming:\n");
    DisplayMatrix(tmp_matrix, stdout);
    increment_cnt(params->counters->cons);
    sem_post(&mutex);
    sem_post(&empty);

  }
  return NULL;
}
