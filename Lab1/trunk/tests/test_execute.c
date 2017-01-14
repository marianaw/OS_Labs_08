#include "test_execute.h"
#include <check.h>  /* modulo unit testing */
#include <signal.h> /* para los SIG_ */
#include <assert.h> /* para las asserciones */
#include <string.h> /* para strcmp */
#include <stdlib.h> /* para fmemopen y otros */
#include <stdio.h>  /* para manejo de archivos y otros */

#include "execute.h"
#include "path_transform.h"
#include "parser.h"
#include "command.h"

/* Variables para los test y setup */
static pipeline *test_pl		= NULL;
static FILE *test_file	= NULL;
static bstring test_bs1 = NULL;
static char *test_str	= NULL;

/* Pruebas generales */

/* Inicializador general */
static void setup (void) 
{
	test_str = bdata ( test_bs1);
	assert( test_str != NULL );
	
	test_file = fmemopen (test_str , strlen (test_str ), "r");  
	assert( test_file != NULL);
	
	test_pl  = parsear ( test_file );
	assert (test_pl != NULL);
	
	assert ( path_transform ( test_pl ) );
}

/* Inicializado absolutas */
static void setup_absolute (void) 
{
	test_bs1	= bfromcstr ( "echo    \"chaja\"     \n");
	setup();
}

/* Inicializado Relativas */
static void setup_relative (void) 
{
	test_bs1	= bfromcstr ( "ls\n");
	
	setup();
}

/* Inicilizado $PATH */
static void setup_PATH (void) 
{
	test_bs1	= bfromcstr ( "exit\n" );
	setup();
}

/* Destructor general */
static void teardown (void) 
{
	
	if( test_pl != NULL)
	{
		pipeline_destroy ( test_pl);
		test_pl= NULL;
	}
	
	if( test_bs1 != NULL)
	{
		bdestroy ( test_bs1);
		test_bs1 = NULL;
	}
	
	if (test_file != NULL)
	{
		fclose (test_file);
		test_file = NULL;
	}
}

/*testeo de precondiciones */
START_TEST (test_exec_null )
{
	execute (NULL);
}
END_TEST

START_TEST (test_exec)
{
	printf ("\nEjecutando\n");
	fail_unless ( execute ( test_pl ) );
	printf ("\n%s\n" , getenv ( "PWD" ));
}
END_TEST

Suite *execute_suite (void)
{
	Suite *s = suite_create ("execute");
	TCase *tc_preconditions	 = tcase_create ("Precondition");
	TCase *tc_funct_absolute = tcase_create ("Functionality: ONE_Absolute_Path");
	TCase *tc_funct_relative = tcase_create ("Functionality: ONE_Relative_Path");
	TCase *tc_funct_PATH	 = tcase_create ("Functionality: ONE_$PATH_Path");
	
	
	/* Precondiciones */
	tcase_add_test_raise_signal (tc_preconditions, test_exec_null , SIGABRT);
	suite_add_tcase (s, tc_preconditions);
	/* Funcionalidad */
	
	/* Testeo de rutas absolutas*/
	tcase_add_checked_fixture (tc_funct_absolute, setup_absolute, teardown);
	tcase_add_test (tc_funct_absolute, test_exec);
	suite_add_tcase (s, tc_funct_absolute);
	
	/* Testeo de rutas relativas*/
	tcase_add_checked_fixture (tc_funct_relative, setup_relative, teardown);
	tcase_add_test (tc_funct_relative, test_exec);
	suite_add_tcase (s, tc_funct_relative);
	
	/* Testeo de rutas de $PATH */
	tcase_add_checked_fixture (tc_funct_PATH, setup_PATH, teardown);
	tcase_add_test (tc_funct_PATH, test_exec);
	suite_add_tcase (s, tc_funct_PATH);
	
	return s;
}
