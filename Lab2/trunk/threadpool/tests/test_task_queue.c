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

/* $Rev: 558 $ */
#define  _BSD_SOURCE
#include <stdlib.h> /* setenv */
#include <string.h> /* strcmp */
#include <check.h>  /* fail_unless */
#include <assert.h> /* assert */
#include <mcheck.h> /* mtrace, muntrace */
#include <signal.h> /* SIGABRT */
#include <unistd.h> /* unlink */

#include "test_task_queue.h"
#include "task_queue.h"

#define QSIZE 10

task_queue_t test_queue = NULL, temp = NULL;

static void set_up(void) {
	test_queue = task_queue_create(QSIZE);
}

static void tear_down(void) {
	assert(test_queue != NULL); /* check PRE */
	test_queue = task_queue_destroy(test_queue);
}

/* create, destroy */
START_TEST (test_create) {
	test_queue = task_queue_create(QSIZE);
	fail_unless(test_queue != NULL);
	test_queue = task_queue_destroy(test_queue);
}
END_TEST

START_TEST (test_destroy) {
	test_queue = task_queue_create(QSIZE);
	test_queue = task_queue_destroy(test_queue);
	fail_unless(test_queue == NULL);
}
END_TEST

/* preconditions */
START_TEST (test_queue_size_zero) {
	test_queue = task_queue_create(0);
}
END_TEST

START_TEST (test_destroy_self_null) {
	test_queue = task_queue_destroy(NULL);
}
END_TEST

/*START_TEST (test_execute_task_self_null) {
	task_queue_execute_task(NULL, NULL, NULL);
}
END_TEST

START_TEST (test_execute_task_routine_null) {
	test_queue = task_queue_create(PSIZE, QSIZE);
	task_queue_execute_task(test_queue, NULL, NULL);
	test_queue = task_queue_destroy(test_queue);
}
END_TEST*/

/* functionality */
START_TEST (test_create_returns_not_null) {
	fail_unless(test_queue != NULL);
}
END_TEST

START_TEST (test_destroy_returns_null) {
	temp = task_queue_create(QSIZE);
	fail_unless(task_queue_destroy(temp) == NULL);
}
END_TEST

#define MALLOC_TRACE_ENV "MALLOC_TRACE"
#define MTRACE_LOG "mtrace.log"
#define MTRACE_OUTPUT "mtrace.out"

START_TEST (test_no_memory_leaks) { /* FIXME: under construction */
	assert(setenv(MALLOC_TRACE_ENV, MTRACE_LOG, 1) == 0);
	assert(strcmp(getenv(MALLOC_TRACE_ENV), MTRACE_LOG) == 0);
	
	mtrace();
	temp = task_queue_create(QSIZE);
	temp = task_queue_destroy(temp);
	muntrace();
	
	assert(system("mtrace runner " MTRACE_LOG "> " MTRACE_OUTPUT) != -1);
	/*fail_unless(there are no memory leak);*/
	unlink(MTRACE_LOG);
	unlink(MTRACE_OUTPUT);
}
END_TEST

/* test suite */

Suite *task_queue_suite(void) {
	Suite *s = suite_create("task_queue");
	TCase *tc_create_destroy = tcase_create("create_destroy");
	TCase *tc_preconditions = tcase_create("preconditions");
	TCase *tc_functionality = tcase_create("functionality");
	TCase *tc_memory = tcase_create("memory");

	tcase_add_checked_fixture(tc_create_destroy, NULL, NULL);
	tcase_add_test(tc_create_destroy, test_create);
	tcase_add_test(tc_create_destroy, test_destroy);
	suite_add_tcase(s, tc_create_destroy);

	tcase_add_checked_fixture(tc_preconditions, NULL, NULL);
	tcase_add_test_raise_signal(tc_preconditions, test_queue_size_zero, SIGABRT);
	tcase_add_test_raise_signal(tc_preconditions, test_destroy_self_null, SIGABRT);
	suite_add_tcase(s, tc_preconditions);

	tcase_add_checked_fixture(tc_functionality, set_up, tear_down);
	tcase_add_test(tc_functionality, test_create_returns_not_null);
	tcase_add_test(tc_functionality, test_destroy_returns_null);
	suite_add_tcase(s, tc_functionality);

	tcase_add_checked_fixture(tc_memory, NULL, NULL);
	tcase_add_test(tc_memory, test_no_memory_leaks);
	suite_add_tcase(s, tc_memory);

	return s;
}
