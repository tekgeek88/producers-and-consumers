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
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int use_ptr   = 0;
int fill_ptr  = 0;

// Producer consumer data structures
counter_t *counter;

// Bounded buffer bigmatrix defined in prodcons.h
Matrix ** bigmatrix;

void init_buffer_size_counter() {
  counter = (counter_t*) malloc(sizeof(counter_t));
  init_cnt(counter);
}

// Bounded buffer put() get()
int put(Matrix *value, void *args) {
  thread_args_t *params = (thread_args_t*) args;
  fill_ptr = get_cnt(params->counters->prod) % BOUNDED_BUFFER_SIZE;
  bigmatrix[fill_ptr] = value;
  increment_cnt(params->counters->prod);
  increment_cnt(counter);
}

Matrix *get(void *args) {
  thread_args_t *params = (thread_args_t*) args;
  use_ptr = get_cnt(params->counters->cons);
  Matrix *tmp_matrix = bigmatrix[use_ptr];
  increment_cnt(params->counters->cons);
  decrement_cnt(counter);
  return tmp_matrix;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg) {
  printf("prod_worker is running!\n");
  thread_args_t *params = (thread_args_t*) arg;

#if OUTPUT
  printf("In worker...\n");
  printf("loops=%d\n", NUMBER_OF_MATRICES);
#endif
  int i;
  for (i = 0; i < NUMBER_OF_MATRICES; i++) {
    Matrix *value = GenMatrixRandom();
    pthread_mutex_lock(&mutex);
    while (get_cnt(counter) == BOUNDED_BUFFER_SIZE) {
      pthread_cond_wait(&empty, &mutex);
    }

#if OUTPUT
    DisplayMatrix(value, stdout);
#endif
    put(value, params);
    pthread_cond_signal(&fill);
    pthread_mutex_unlock(&mutex);
  }
  params->prodConStats->matrixtotal = get_cnt(params->counters->prod);
  return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg) {
  thread_args_t *params = (thread_args_t *) arg;
  Matrix *multiplied = NULL;
  Matrix *matrix_A = NULL;
  Matrix *matrix_B = NULL;

  while (get_cnt(params->counters->cons) < NUMBER_OF_MATRICES) {
    pthread_mutex_lock(&mutex);

    while (get_cnt(counter) == 0) {
      pthread_cond_wait(&fill, &mutex);
    }


    Matrix *matrix_A = get(arg);
    if (NULL == matrix_A) {
      printf("");
    }

    while (get_cnt(counter) == 0 && get_cnt(params->counters->cons) < NUMBER_OF_MATRICES) {
      pthread_cond_wait(&fill, &mutex);
    }
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);

    use_ptr = get_cnt(params->counters->cons) % BOUNDED_BUFFER_SIZE;
    if (get_cnt(params->counters->prod) - get_cnt(params->counters->cons) > 0) {
      multiplied = MatrixMultiply(matrix_A, bigmatrix[use_ptr]);
    }
    if (multiplied != NULL) {
      pthread_mutex_lock(&mutex);
      Matrix *matrix_B = get(arg);
      pthread_cond_signal(&empty);
      pthread_mutex_unlock(&mutex);
      params->prodConStats->multtotal++;
      params->prodConStats->sumtotal += SumMatrix(multiplied);

#if OUTPUT
      DisplayMatrix(matrix_A, stdout);
      printf("    X\n");
      DisplayMatrix(matrix_B, stdout);
      printf("    =\n");
      DisplayMatrix(multiplied, stdout);
      printf("\n");
      printf("----------------------------\n");
#endif
      if (matrix_B != NULL) {
      FreeMatrix(matrix_B);
      matrix_B = NULL;
      }
      if (multiplied != NULL) {
      FreeMatrix(multiplied);
      multiplied = NULL;
      }
    }

    if (matrix_A != NULL) {
      FreeMatrix(matrix_A);
      matrix_A = NULL;
    }

  }

  return NULL;
}
