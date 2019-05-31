/*
 *  Signal example
 *  Based on Operating Systems: Three Easy Pieces by R. Arpaci-Dusseau and A. Arpaci-Dusseau
 *
 *  This example uses locks and a condition to synchronize a child thread and
 *  the parent.  The child generates matrices and the parent computes the
 *  average when the matrix is ready for processing.
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2019
 */

// Include libraries required for this module only
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"

// SYNCHRONIZED COUNTER METHOD IMPLEMENTATION
// Based on Three Easy Pieces

void init_cnt(counter_t *c)  {
  c->value = 0;
  pthread_mutex_init(&c->lock, NULL);
}

void init_counters(counters_t *counters) {
  counters->cons = (counter_t*)malloc(sizeof(counter_t));
  counters->prod = (counter_t*)malloc(sizeof(counter_t));
  init_cnt(counters->cons);
  init_cnt(counters->prod);
}

void decrement_cnt(counter_t *c)  {
  pthread_mutex_lock(&c->lock);
  c->value--;
  pthread_mutex_unlock(&c->lock);
}

void increment_cnt(counter_t *c)  {
  pthread_mutex_lock(&c->lock);
  c->value++;
  pthread_mutex_unlock(&c->lock);
}

int get_cnt(counter_t *c)  {
  pthread_mutex_lock(&c->lock);
  int rc = c->value;
  pthread_mutex_unlock(&c->lock);
  return rc;
}
