/*
 *  prodcons header
 *  Function prototypes, data, and constants for producer/consumer module
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2019
 */

#ifndef PRODCONS_H
#define PRODCONS_H
#include "counter.h"


Matrix ** bigmatrix;

// PRODUCER-CONSUMER put() get() function prototypes

// Data structure to track matrix production / consumption stats
// sumtotal - total of all elements produced or consumed
// multtotal - total number of matrices multiplied
// matrixtotal - total number of matrices produced or consumed
typedef struct prodcons {
  int sumtotal;
  int multtotal;
  int matrixtotal;
} ProdConsStats;

typedef struct thread_args {
    counters_t *counters;
    ProdConsStats *prodConStats;
} thread_args_t;

// PRODUCER-CONSUMER thread method function prototypes
void *prod_worker(void *arg);
void *cons_worker(void *arg);

// Routines to add and remove matrices from the bounded buffer
int put(Matrix *value, void *args);
Matrix * get(void*);
void init_buffer_size_counter();


#endif //PROCON_PROCON_H