/*
 *  counter header
 *  Function prototypes, data, and constants for synchronized counter module
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2019
 */

// SYNCHRONIZED COUNTER
#ifndef COUNTER_H
#define COUNTER_H

// counter structures
// Comment
typedef struct __counter_t {
  int value;
  pthread_mutex_t  lock;
} counter_t;

typedef struct __counters_t {
  counter_t * prod;
  counter_t * cons;
} counters_t;

// counter methods
void init_cnt(counter_t *c);
void init_counters(counters_t *c);
void decrement_cnt(counter_t *c);
void increment_cnt(counter_t *c);
int get_cnt(counter_t *c);

#endif