/*
* Copyright (C) 2008 Natalia B. Bidart
* 
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
* 
* You should have received a copy of the GNU General Public License along
* with this library; if not, see <http://www.gnu.org/licenses/>.
*/

/* $Rev: 530 $ */
#include <stdbool.h> /* bool, true, false */
#include <assert.h> /* assert */
#include <stdlib.h> /* calloc, free, strtoull */
#include <math.h> /* ceil, sqrt, fmod */
#include <pthread.h> /* pthread_mutex_{t, init, destroy} */
                     /* pthread_cond_{t, init, destroy, wait, signal} */
#include <semaphore.h> /* sem_{t, init, destroy, wait, post} */
#include "utils.h" /* debug */
#include "thread_pool.h" /* thread_pool_{t, create, destroy, execute_task} */
#include "primality_checker.h" /* is_prime */

#define BOTTOM 2
#define CHUNK_SIZE 1000

/* shared data */
unsigned long long int candidate = 0, intervals = 0, tested = 0;
bool prime_found = true;
pthread_mutex_t mutex;
sem_t done;

/* testing routine's prototype */
static void *check_interval(void *arg);

/* helper struct to pass useful arguments to testing routine */
struct range {
	unsigned long long int start;
	unsigned long long int end;
};

/* main function */
bool is_prime(unsigned long long int a_candidate,
              unsigned int pool_size, unsigned int queue_size) {
	unsigned int i = 0;
	double q = 0.0;
	struct range *tests = NULL;
	thread_pool_t p = NULL;
	
	/* PRE: */assert(a_candidate > 0);
	/* PRE: */assert(pool_size > 0);
	/* PRE: */assert(queue_size > 0);
	
	candidate = a_candidate;
	assert(candidate == a_candidate);
	
	debug("candidate -> %lli\n", candidate);
	/* from: http://en.wikipedia.org/wiki/Primality_testing
	 *
	 * Rather than testing all numbers up to `candidate - 1', we need
	 * only test numbers up to `sqrt(candidate)': if `candidate'
	 * is composite then it can be factored into two values, at least one
	 * of which must be less than or equal to `sqrt(candidate)'.
	 */
	q = sqrt(candidate);
	
	/* divide work for threads */
	intervals = ceil(q / CHUNK_SIZE);
	
	/* init stuff */
	pthread_mutex_init(&mutex, NULL);
	sem_init(&done, 0, 0);
	
	tests = calloc(intervals, sizeof(struct range));
	assert(tests != NULL);
	
	p = thread_pool_create(pool_size, queue_size);
	assert(p != NULL);
	
	tested = 0;
	prime_found = true;
	
	/* execute tasks through thread pool `p' */
	for (i = 0; i < intervals && prime_found; i++) {
		tests[i].start = BOTTOM + i * CHUNK_SIZE;
		if (i == intervals - 1) { /* last interval */
			tests[i].end = ceil(q) + 1; /* the nearest integer to `q' */
		} else {
			tests[i].end = tests [i].start + CHUNK_SIZE;
		}
		thread_pool_execute_task(p, check_interval, (void *)&tests[i]);
	}
	
	/* wait for all intervals to be processed */
	debug("waiting for done!\n");
	sem_wait(&done);
	
	/* destroy stuff */
	p = thread_pool_destroy(p);
	pthread_mutex_destroy(&mutex);
	sem_destroy(&done);
	free(tests);
	tests = NULL;
	
	return(prime_found);
}

/* testing routine */
static void *check_interval(void *arg) {
	struct range interval;
	unsigned int i = 0;
	
	interval = *(struct range *)arg;
	debug("interval -> (%lli, %lli)\n", interval.start, interval.end);
	for (i = interval.start; i < interval.end && prime_found; i++){
		if (i != candidate && (candidate % i) == 0) {
			debug("divisor found -> %u\n", i);
			prime_found = false;
		}
	}
	assert(i >= interval.end || !prime_found);
	pthread_mutex_lock(&mutex);
	tested++;
	pthread_mutex_unlock(&mutex);
	
	if (!prime_found || tested == intervals) {
		debug("signaling done!\n");
		sem_post(&done);
	}
	
	return NULL;
}
