/*
* Copyright (C) 2007 Natalia B. Bidart
* 
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2.1 of the License, or any
* later version.
* 
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
* 
* You should have received a copy of the GNU General Public License along
* with this library; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

/* $Rev: 530 $ */
#include <stdio.h> /* printf */
#include <stdbool.h> /* bool, true, false */
#include <assert.h> /* assert */
#include <stdlib.h> /* calloc, free, strtoull */
#include "primality_checker.h" /* thread_pool_{t, create, destroy, execute_task} */

#include "task_queue.h"

#define POOL_SIZE 2
#define QUEUE_SIZE 100

/* main routine */
int main(int argc, char *argv[]) {
	unsigned long long int number_to_test = 0;
	task_queue_t tas;
	task_t t;
	
	printf ("hola0\n");
	t=task_create( (void *) is_prime, NULL);
	printf ("hola00\n");
	tas= task_queue_create (1);
	tas= task_queue_enqueue (tas, t);
	
	printf ("hola1\n");
	task_queue_print (tas);
	printf ("hola2\n");
	
	/* grab number to test from arguments */
	if (argc < 2) {
		printf("Usage: test_thread_pool <number to test for primality>.\n");
		return(-1);
	}
	assert(argc > 1);
	number_to_test = strtoull(argv[1], NULL, 10);
	/* one of the longuest primes: */
	/*number_to_test = 2305843009213693951ULL;*/
	if (number_to_test < 1) {
		printf("Please enter a positive number.\n");
		return(-1);
	}
	
	/* print result on stdout */
	printf("test_thread_pool.c: main: %lli is %sprime!\n",
	        number_to_test, (is_prime(number_to_test, POOL_SIZE, QUEUE_SIZE) ? "" : "not "));
	
	return(0);
}
