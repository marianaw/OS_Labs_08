#include "test_path_transform.h"
#include <check.h>  /* modulo unit testing */
#include <signal.h> /* para los SIG_ */
#include <assert.h> /* para las asserciones */
#include <string.h> /* para strcmp */
#include <stdlib.h> /* para fmemopen y otros */
#include <stdio.h>  /* para manejo de archivos y otros */

#include "command.h"
#include "parser.h"
#include "path_transform.h"

/* Variables para los test y setup */
static pipeline *pl		= NULL;
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
}

/* Inicializado absolutas */
static void setup_absolute (void) 
{
	test_bs1	= bfromcstr ( "/bin/ls\n");
	setup();
}

/* Inicializado Relativas */
static void setup_relative (void) 
{
	test_bs1	= bfromcstr ( "../ls\n");
	setup();
}

/* Inicilizado $PATH */
static void setup_PATH (void) 
{
	test_bs1	= bfromcstr ( "ls   -x         -color=yes\n");
	setup();
}

/* Inicilizado pipe_mix */
static void setup_pipe_mix (void) 
{
	test_bs1	= bfromcstr ( " /bin/ls | wc  | ../ls\n");
	setup();
}

/* Inicilizado redirecciones ( los archivos deben existir ) */
static void setup_redir (void) 
{
	test_bs1	= bfromcstr ( "exit\n");
	setup();
}

/* Inicilizado builtin */
static void setup_builtin (void) 
{
	test_bs1	= bfromcstr ( " cd\n");
	setup();
}

static void teardown (void) 
{
	
	if( pl != NULL)
	{
		pipeline_destroy ( pl);
		pl= NULL;
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
START_TEST (test_pipe_null )
{
	path_transform (NULL);
}
END_TEST

/*testeo de funcionalidad */
START_TEST (test_path_find )
{
	
	pl  = parsear ( test_file );
	
	assert ( pl != NULL);
	
	test_bs1 = pipeline_to_string ( pl );
	
	fail_unless (pl != NULL);
	
	fail_unless ( path_transform ( pl ) );
	
	test_bs1 = pipeline_to_string ( pl );
	
	printf ( "Path : %s\n" , bdata ( test_bs1));
}
END_TEST

Suite *path_transform_suite (void)
{
	Suite *s = suite_create ("path_transform");
	TCase *tc_preconditions	 = tcase_create ("Precondition");
	TCase *tc_funct_absolute = tcase_create ("Functionality: Absolute_Path");
	TCase *tc_funct_relative = tcase_create ("Functionality: Relative_Path");
	TCase *tc_funct_PATH	 = tcase_create ("Functionality: $PATH_Path");
	TCase *tc_funct_pipe_mix = tcase_create ("Functionality: Pipe_Mix");
	TCase *tc_funct_redir = tcase_create	("Functionality: Redirections");
	TCase *tc_funct_builtin	 = tcase_create ("Functionality: builtin");
	
	
	/* Precondiciones */
	tcase_add_test_raise_signal (tc_preconditions, test_pipe_null , SIGABRT);
	suite_add_tcase (s, tc_preconditions);

	/* Funcionalidad */
	
	/* Testeo de rutas absolutas*/
	tcase_add_checked_fixture (tc_funct_absolute, setup_absolute, teardown);
	tcase_add_test (tc_funct_absolute, test_path_find);
	suite_add_tcase (s, tc_funct_absolute);
	
	/* Testeo de rutas relativas*/
	tcase_add_checked_fixture (tc_funct_relative, setup_relative, teardown);
	tcase_add_test (tc_funct_relative, test_path_find);
	suite_add_tcase (s, tc_funct_relative);
	
	/* Testeo de rutas de $PATH */
	tcase_add_checked_fixture (tc_funct_PATH, setup_PATH, teardown);
	tcase_add_test (tc_funct_PATH, test_path_find);
	suite_add_tcase (s, tc_funct_PATH);
	
	/* Testeo de pipe mix */
	tcase_add_checked_fixture (tc_funct_pipe_mix, setup_pipe_mix, teardown);
	tcase_add_test (tc_funct_pipe_mix, test_path_find);
	suite_add_tcase (s, tc_funct_pipe_mix);
	
	/* Testeo de redirecciones */
	tcase_add_checked_fixture (tc_funct_redir, setup_redir, teardown);
	tcase_add_test (tc_funct_redir, test_path_find);
	suite_add_tcase (s, tc_funct_redir);
	
	/* Testeo de builtin */
	tcase_add_checked_fixture (tc_funct_builtin, setup_builtin, teardown);
	tcase_add_test (tc_funct_builtin, test_path_find);
	suite_add_tcase (s, tc_funct_builtin);

	return s;
}

