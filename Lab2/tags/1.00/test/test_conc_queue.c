#define  _BSD_SOURCE
#include <check.h>  /* fail_unless */
#include <mcheck.h> /* mtrace, muntrace */
#include <signal.h> /* SIGABRT */
#include <assert.h> /* assert */
#include <stdlib.h> /* setenv */
#include <string.h> /*strcmp */
#include <unistd.h> /* unlink */

#include "test_conc_queue.h"
#include "conc_queue.h"


conc_queue_t test_conc_queue = NULL, conc_temp = NULL;

/* Funciones de seteo y limpieza */
static void set_up(void) 
{
	test_conc_queue = conc_queue_create();
}

static void tear_down(void) 
{
	assert(test_conc_queue != NULL); /* check PRE */
	test_conc_queue = conc_queue_destroy(test_conc_queue);
}

/* create, destroy */
START_TEST (test_create) 
{
	test_conc_queue = conc_queue_create();
	fail_unless(test_conc_queue != NULL);
	test_conc_queue = conc_queue_destroy(test_conc_queue);
}
END_TEST

START_TEST (test_destroy) 
{
	test_conc_queue = conc_queue_create();
	test_conc_queue = conc_queue_destroy(test_conc_queue);
	fail_unless(test_conc_queue == NULL);
}
END_TEST


/* preconditions */
START_TEST (test_destroy_self_null) 
{
	test_conc_queue = conc_queue_destroy(NULL);
}
END_TEST

/* functionality */
START_TEST (test_create_returns_not_null) 
{
	fail_unless(test_conc_queue != NULL);
}
END_TEST

START_TEST (test_destroy_returns_null) 
{
	conc_temp = conc_queue_create();
	fail_unless(conc_queue_destroy(conc_temp) == NULL);
}
END_TEST


/* memory */

#define MALLOC_TRACE_ENV "MALLOC_TRACE"
#define MTRACE_LOG "mtrace.log"
#define MTRACE_OUTPUT "mtrace.out"

START_TEST (test_no_memory_leaks) { /* FIXME: under construction */
	assert(setenv(MALLOC_TRACE_ENV, MTRACE_LOG, 1) == 0);
	assert(strcmp(getenv(MALLOC_TRACE_ENV), MTRACE_LOG) == 0);
	
	mtrace();
	conc_temp = conc_queue_create();
	conc_temp = conc_queue_destroy(conc_temp);
	muntrace();
	
	assert(system("mtrace runner " MTRACE_LOG "> " MTRACE_OUTPUT) != -1);
	/*fail_unless(there are no memory leak);*/
	unlink(MTRACE_LOG);
	unlink(MTRACE_OUTPUT);
}
END_TEST


Suite *conc_queue_suite(void) 
{
	Suite *s = suite_create("conc_queue");
	TCase *tc_create_destroy = tcase_create("create_destroy");
	TCase *tc_preconditions = tcase_create("preconditions");
	TCase *tc_functionality = tcase_create("functionality");
	TCase *tc_memory = tcase_create("memory");
	
	/* create, destroy */
	tcase_add_checked_fixture(tc_create_destroy, NULL, NULL);
	tcase_add_test(tc_create_destroy, test_create);
	tcase_add_test(tc_create_destroy, test_destroy);
	suite_add_tcase(s, tc_create_destroy);
	
	/* preconditions */
	tcase_add_checked_fixture(tc_preconditions, NULL, NULL);
	tcase_add_test_raise_signal(tc_preconditions, test_destroy_self_null, SIGABRT);
	suite_add_tcase(s, tc_preconditions);
	
	/* functionality */
	tcase_add_checked_fixture(tc_functionality, set_up, tear_down);
	tcase_add_test(tc_functionality, test_create_returns_not_null);
	tcase_add_test(tc_functionality, test_destroy_returns_null);
	suite_add_tcase(s, tc_functionality);
	
	/* memory */
	tcase_add_checked_fixture(tc_memory, NULL, NULL);
	tcase_add_test(tc_memory, test_no_memory_leaks);
	suite_add_tcase(s, tc_memory);
	
	return s; 
}
