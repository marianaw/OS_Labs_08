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
#include <check.h>  /* fail_unless */
#include <mcheck.h> /* mtrace, muntrace */
#include <signal.h> /* SIGABRT */
#include <assert.h> /* assert */
#include <stdlib.h> /* setenv */
#include <string.h> /*strcmp */
#include <unistd.h> /* unlink */

#include "test_thread_pool.h"
#include "thread_pool.h"

#define PSIZE 2
#define QSIZE 10

thread_pool_t test_pool = NULL, tmp = NULL;
int executed = 0;

static void set_up(void) {
	executed = 0;
	test_pool = thread_pool_create(PSIZE, QSIZE);
}

static void tear_down(void) {
	assert(test_pool != NULL); /* check PRE */
	test_pool = thread_pool_destroy(test_pool);
}

static void *dummy_task(void *arg) {
	executed += 1;
	return NULL;
}

/* create, destroy */
START_TEST (test_create) {
	test_pool = thread_pool_create(PSIZE, QSIZE);
	fail_unless(test_pool != NULL);
	test_pool = thread_pool_destroy(test_pool);
}
END_TEST

START_TEST (test_destroy) {
	test_pool = thread_pool_create(PSIZE, QSIZE);
	test_pool = thread_pool_destroy(test_pool);
	fail_unless(test_pool == NULL);
}
END_TEST

/* preconditions */
START_TEST (test_pool_size_zero) {
	test_pool = thread_pool_create(0, QSIZE);
}
END_TEST

START_TEST (test_queue_size_zero) {
	test_pool = thread_pool_create(PSIZE, 0);
}
END_TEST

START_TEST (test_destroy_self_null) {
	test_pool = thread_pool_destroy(NULL);
}
END_TEST

START_TEST (test_execute_task_self_null) {
	thread_pool_execute_task(NULL, NULL, NULL);
}
END_TEST

START_TEST (test_execute_task_routine_null) {
	test_pool = thread_pool_create(PSIZE, QSIZE);
	thread_pool_execute_task(test_pool, NULL, NULL);
	test_pool = thread_pool_destroy(test_pool);
}
END_TEST

START_TEST (test_execute_task_arg_null) {
	test_pool = thread_pool_create(PSIZE, QSIZE);
	thread_pool_execute_task(test_pool, dummy_task, NULL);
	test_pool = thread_pool_destroy(test_pool);
}
END_TEST

/* functionality */
START_TEST (test_create_returns_not_null) {
	fail_unless(test_pool != NULL);
}
END_TEST

START_TEST (test_destroy_returns_null) {
	tmp = thread_pool_create(PSIZE, QSIZE);
	fail_unless(thread_pool_destroy(tmp) == NULL);
}
END_TEST

START_TEST (test_execute_task_executes) {
	tmp = thread_pool_create(PSIZE, QSIZE);
	thread_pool_execute_task(tmp, dummy_task, NULL);
	sleep(1); /* give thread pool time to execute the task */
	tmp = thread_pool_destroy(tmp);
	fail_unless(executed == 1);
}
END_TEST

#define MALLOC_TRACE_ENV "MALLOC_TRACE"
#define MTRACE_LOG "mtrace.log"
#define MTRACE_OUTPUT "mtrace.out"

START_TEST (test_no_memory_leaks) { /* FIXME: under construction */
	assert(setenv(MALLOC_TRACE_ENV, MTRACE_LOG, 1) == 0);
	assert(strcmp(getenv(MALLOC_TRACE_ENV), MTRACE_LOG) == 0);
	
	mtrace();
	tmp = thread_pool_create(PSIZE, QSIZE);
	tmp = thread_pool_destroy(tmp);
	muntrace();
	
	assert(system("mtrace runner " MTRACE_LOG "> " MTRACE_OUTPUT) != -1);
	/*fail_unless(there are no memory leak);*/
	unlink(MTRACE_LOG);
	unlink(MTRACE_OUTPUT);
}
END_TEST

/* test suite */

Suite *thread_pool_suite(void) {
	Suite *s = suite_create("thread_pool");
	TCase *tc_create_destroy = tcase_create("create_destroy");
	TCase *tc_preconditions = tcase_create("preconditions");
	TCase *tc_functionality = tcase_create("functionality");
	TCase *tc_memory = tcase_create("memory");

	tcase_add_checked_fixture(tc_create_destroy, NULL, NULL);
	tcase_add_test(tc_create_destroy, test_create);
	tcase_add_test(tc_create_destroy, test_destroy);
	suite_add_tcase(s, tc_create_destroy);

	tcase_add_checked_fixture(tc_preconditions, NULL, NULL);
	tcase_add_test_raise_signal(tc_preconditions, test_pool_size_zero, SIGABRT);
	tcase_add_test_raise_signal(tc_preconditions, test_queue_size_zero, SIGABRT);
	tcase_add_test_raise_signal(tc_preconditions, test_destroy_self_null, SIGABRT);
	tcase_add_test_raise_signal(tc_preconditions, test_execute_task_self_null, SIGABRT);
	tcase_add_test_raise_signal(tc_preconditions, test_execute_task_routine_null, SIGABRT);
	tcase_add_test(tc_preconditions, test_execute_task_arg_null);
	suite_add_tcase(s, tc_preconditions);

	tcase_add_checked_fixture(tc_functionality, set_up, tear_down);
	tcase_add_test(tc_functionality, test_create_returns_not_null);
	tcase_add_test(tc_functionality, test_destroy_returns_null);
	tcase_add_test(tc_functionality, test_execute_task_executes);
	suite_add_tcase(s, tc_functionality);

	tcase_add_checked_fixture(tc_memory, NULL, NULL);
	tcase_add_test(tc_memory, test_no_memory_leaks);
	suite_add_tcase(s, tc_memory);

	return s;
}
