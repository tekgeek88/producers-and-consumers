/*
 *  pcmatrix module
 *  Primary module providing control flow for the pcMatrix program
 *
 *  Producer consumer bounded buffer program to produce random matrices in parallel
 *  and consume them while searching for valid pairs for matrix multiplication.
 *  Matrix multiplication requires the first matrix column count equal the
 *  second matrix row count.
 *
 *  A matrix is consumed from the bounded buffer.  Then matrices are consumed
 *  from the bounded buffer, one at a time, until an eligible matrix for multiplication
 *  is found.
 *
 *  Totals are tracked using the ProdConsStats Struct for:
 *  - the total number of matrices multiplied (multtotal from consumer threads)
 *  - the total number of matrices produced (matrixtotal from producer threads)
 *  - the total number of matrices consumed (matrixtotal from consumer threads)
 *  - the sum of all elements of all matrices produced and consumed (sumtotal from producer and consumer threads)
 *
 *  Correct programs will produce and consume the same number of matrices, and
 *  report the same sum for all matrix elements produced and consumed.
 *
 *  Each thread produces a total sum of the value of
 *  randomly generated elements.  Producer sum and consumer sum must match.
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "counter.h"
#include "prodcons.h"
#include "pcmatrix.h"
#include <semaphore.h>

void init_ProdConStats(ProdConsStats *pcs);

void init_thread_args(thread_args_t *args) {

}


int main(int argc, char *argv[]) {

  // Process command line arguments
  int numw = NUMWORK;

  if (argc == 1) {
    BOUNDED_BUFFER_SIZE = MAX;
    NUMBER_OF_MATRICES = LOOPS;
    MATRIX_MODE = DEFAULT_MATRIX_MODE;
    printf("USING DEFAULTS: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n", numw,
           BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  } else {
    if (argc == 2) {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = MAX;
      NUMBER_OF_MATRICES = LOOPS;
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 3) {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = LOOPS;
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 4) {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = atoi(argv[3]);
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 5) {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = atoi(argv[3]);
      MATRIX_MODE = atoi(argv[4]);
    }
    printf("USING: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n", numw,
           BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  }

  // Create space for the buffer and clear it
  bigmatrix = (Matrix **) malloc(sizeof(Matrix*) *BOUNDED_BUFFER_SIZE);
  int i;
  for (i = 0; i < BOUNDED_BUFFER_SIZE; i++) {
    bigmatrix[i] = 0;
  }

  time_t t;
  // Seed the random number generator with the system time
  srand((unsigned) time(&t));

  // initialize counters
  counters_t *counters = malloc(sizeof(counters_t));
  init_counters(counters);

  // Initialize consumer and producer stats
  ProdConsStats *pcs = malloc(sizeof(ProdConsStats));
  init_ProdConStats(pcs);

  // Initial params for keeping track of threads
  thread_args_t *params = malloc(sizeof(thread_args_t));
  params->counters = counters;
  params->prodConStats = pcs;

  // Initialize the semiphores with the correct buffer size
  init_semiphores(BOUNDED_BUFFER_SIZE);

  //
  // Demonstration code to show the use of matrix routines
  //
  // DELETE THIS CODE ON ASSIGNMENT 2 SUBMISSION
  // ----------------------------------------------------------
  printf("MATRIX MULTIPLICATION DEMO:\n\n");
//    Matrix *m1, *m2, *m3;
//    for (int i = 0; i < NUMBER_OF_MATRICES; i++) {
//        m1 = GenMatrixRandom();
//        m2 = GenMatrixRandom();
//        m3 = MatrixMultiply(m1, m2);
//        if (m3 != NULL) {
//            DisplayMatrix(m1, stdout);
//            printf("    X\n");
//            DisplayMatrix(m2, stdout);
//            printf("    =\n");
//            DisplayMatrix(m3, stdout);
//            printf("\n");
//            FreeMatrix(m3);
//            FreeMatrix(m2);
//            FreeMatrix(m1);
//            m1 = NULL;
//            m2 = NULL;
//            m3 = NULL;
//        }
//    }

  // ----------------------------------------------------------

  printf("Producing %d matrices in mode %d.\n", NUMBER_OF_MATRICES, MATRIX_MODE);
  printf("Using a shared buffer of size=%d\n", BOUNDED_BUFFER_SIZE);
  printf("With %d producer and consumer thread(s).\n", numw);
  printf("\n");


  pthread_t pid; //, cid[CMAX]; // A producer and consumer thread
  pthread_t cid;

  // Create the producer thread
  pthread_create(&pid, NULL, prod_worker, params);

  // Create the consumer thread
  pthread_create(&cid, NULL, cons_worker, params);

  pthread_join(pid, NULL);

  // consume ProdConsStats from producer and consumer threads
  // add up total matrix stats in prs, cos, prodtot, constot, consmul
  displayStats(params);

  printf("Finished running program!");

  return 0;
}

void init_ProdConStats(ProdConsStats *pcs) {
  pcs->sumtotal = 0;
  pcs->multtotal = 0;
  pcs->matrixtotal = 0;
}

void displayStats(const thread_args_t *params) {// Variables used to display the output
  int prs = params->counters->prod->value;
  int cos = params->counters->cons->value;
  int prodtot = params->prodConStats->matrixtotal;
  int constot = params->prodConStats->multtotal;
  int consmul = params->prodConStats->sumtotal;

  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", prs, cos);
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", prodtot, constot, consmul);
}