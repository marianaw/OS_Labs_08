
/* Librerias */
#include "path_transform.h"
#include "lexer/lexer.h" /* Para manipular la información obtenida de $PATH */
#include "bstring/bstrlib.h" /* Para las Funciones Principales y Auxiliares */
#include <unistd.h> /* Para utilizar access () y sus constantes simbolicas */
#include <errno.h>  /* Agrega constantes simbolicas para manejo de errores */
#include <stdlib.h> /* Para utilizar getenv() y otras de Acceso al Entorno  */
#include <stdio.h>  /* Para manejo de archivos y otros */
#include <stdbool.h> /* para tener bool */
#include <assert.h> /* Para el manejo de Aserciones */

/* Constantes para el manejo del Lexer , solo simplifican la sintaxis */
#define SPACER ":" /* Elemento que divide las rutas */


/* Declaración de Funciones Internas */
 
static bool find_command_path ( bstring bs, char *pwd);
/* Chequea que el tipo de ruta que es bs. Si esta es relativa o
 * su ruta se encuentra el la variable shell PATH , se modificara
 * bs para convertirlo en una ruta absoluta concatenando la ruta
 * correspondiente. En caso de ser absoluta no se realiza nada.
 * Returns : True si se encuentra el path , en caso de error False.
 *			 Ademas en caso de error , se imprimira un mensaje por
 *			 la salida de errores.
 * Requires : bs != NULL 
 * Ensures : result == "Direccion encontrada"
 */

static bool find_std_path ( bstring bs , int how, char *pwd );
/* En caso de no ser absoluta la direccion bs , se agrega el path.
 * Returns : True si se encuentra el path , en caso de error False.
 *			 Ademas en caso de error , se imprimira un mensaje por
 *			 la salida de errores.
 * Requires :   bs != NULL &&
 *		      (	how == R_OK ||
 *				how == W_OK )	
 * Ensures : result == "Direccion encontrada"
 */ 


bool path_transform (pipeline * pl, char *pwd)
{
	/* Inicialización de variables */
	bool result			= false ; 
	unsigned int i		= 0 ;
	scommand *path_cmd   = NULL;
	bstring path_bs		= NULL;
	
	/* Precondición */
	assert (pl != NULL);
	
	/* Inserta la direccion a cada scommand y los ennvia a la 
	 * cola del pipeline.El ciclo termina al llegar al lenght
	 * o devolviendose un error error */
	for ( i=0; i < pipeline_length ( pl ); i++ )
	{
		/* Tomamos el commando de la cabeza */
		path_cmd = pipeline_front ( pl );
		
		/* En caso de no ser builtin , se busca la dirección 
		 * del commando */
		if ( !scommand_get_builtin ( path_cmd ) )
		{
			result = find_command_path ( scommand_front ( path_cmd ),pwd);
		
			if ( !result ) 
			{   
				path_cmd = NULL;
				return result;
			}
		}
		else
			result = true;
		/* se busca la dirección de la redireción a entrada  */
		path_bs =  scommand_get_redir_in ( path_cmd );
		if ( path_bs != NULL)
		{
			result = find_std_path ( path_bs , R_OK , pwd) ; 
			path_bs = NULL;
			
			if ( !result ) 
			{   
				path_cmd = NULL;
				return result;
			} 
				
		}
		
		/* Se quita la cabeza marcada por path_cmd , la cual luego
		 * es colocada el la cola para poder continuar con el ciclo */
		pipeline_pop_front ( pl );
		pipeline_push_back ( pl , path_cmd );
		path_cmd = NULL;
	}
	
	return result; 
}


/* Funciones Internas */

static bool find_command_path ( bstring bs, char *pwd)
{
	/* Inicialización de variables */
	Lexer *path_lexer   = NULL;
	FILE *path_file		= NULL;
	char *path_str		= NULL;
	bstring path_bs		= NULL;
	bstring aux_bs		= bfromcstr ("/");
	int	error			= 0 ;
	bool result			= true ; 
	/* Precondición */
	assert ( bs != NULL );
	
	/* Si no es una ruta absoluta , entra al if */
	if ( 0 != bstrchrp ( bs , '/' , 0 ) )
	{
		path_bs = NULL;
		/* Si es una ruta relativa , entra al if.
		 * Sino entra al else y busca la informacion en la variable PATH */
		
		if ( BSTR_ERR != bstrrchr ( bs , ( int ) '/' ) )
		{
			
			/* Obtengo la Ruta de la Variable de Entorno PWD */
			/*path_bs = bfromcstr ( getenv ( "PWD" ) );*/
			path_bs = bfromcstr(pwd);
			
			/* Concateno bs a la dirección obtenido del lexer */
			assert ( BSTR_OK == bconcat ( path_bs ,aux_bs ) );
			assert ( BSTR_OK == bconcat ( path_bs , bs ) );
		}
		else
		{
			/* Crea el Lexer con los datos de la variable Shell PATH */
			path_str	= getenv ( "PATH" );
			assert ( path_str != NULL );
			path_file   = fmemopen ( path_str , strlen ( path_str ) , "r");
			assert ( path_file != NULL);
			path_lexer  = lexer_new ( path_file );
			assert ( path_lexer != NULL);
			
			/* Busca la dirección donde se encuentra bs en el Lexer */
			while (!lexer_is_off ( path_lexer )  && path_bs == NULL )
					
			{
				
				/* Busco una dirección en el lexer y la asigno a path_bs */
				lexer_next_to ( path_lexer , SPACER );
				path_bs = lexer_item ( path_lexer );
				assert (path_bs != NULL);
				lexer_skip ( path_lexer , SPACER );
				
				/* Concateno bs a la dirección obtenido del lexer */
				assert ( BSTR_OK == bconcat ( path_bs ,aux_bs ) );
				assert ( BSTR_OK == bconcat ( path_bs , bs ) );
			
				/* Se destruye los datos de ciclos anteriores en path_bs */
				path_str = bdata ( path_bs );
				error = access ( path_str , F_OK && X_OK );
				if ( error )
				{
					assert ( BSTR_ERR != bdestroy ( path_bs ) );
					path_bs = NULL;	
				}
			}
			if ( error )
			{
				perror ( strerror ( error) ) ;
			}
		}
		/* En caso de encontrar una dirección , esta es reemplazada en bs */
		result = (path_bs != NULL);
		if (result )
		{
		
			/* En caso de perder el commando , puede ser a causa de destruir
			 * bs , ver el funcionamiento de bconcat () */
			assert ( BSTR_ERR != bassign ( bs , path_bs ) );
		}

		/* Destruye todo los path_* */
		if (path_lexer != NULL)
		{
			lexer_destroy ( path_lexer );
			path_lexer  = NULL;
		}
		if (path_file != NULL)
		{
			assert( 0 == fclose ( path_file ) );
			path_file = NULL;
		}
		path_str	= NULL;
	}	
	path_bs = NULL;
	bdestroy (aux_bs);
	aux_bs = NULL;
	
	return result ; 
}

static bool find_std_path ( bstring bs , int how, char *pwd ) {
	
	/* Inicialización de variables */
	bool result			= true;
	bstring path_bs		= bs;
	bstring aux_bs		= bfromcstr ("/");
	char * path_str		= NULL;
	int error           = 0;
	
	/* Precondición */
	assert ( bs != NULL &&
		   ( how == R_OK ||
			 how == W_OK ) && pwd != NULL		
					     );
	
	/* Si no es una ruta absoluta entonces le agregare el path
	 * contenido en PWD */
	if ( 0 != bstrrchr ( bs , ( int ) '/' ) )
	{
		/* Obtengo la Ruta de la Variable de Entorno PWD */
		/*path_bs = bfromcstr ( getenv ( "PWD" ) );*/
		path_bs = bfromcstr(pwd);
		
		/* Concateno bs a la dirección obtenido del lexer */
		assert ( BSTR_OK == bconcat ( path_bs ,aux_bs ) );
		assert ( BSTR_OK == bconcat ( path_bs , bs ) );
		
		/* Se destruye los datos de ciclos anteriores en path_bs */
		path_str = bdata ( path_bs );
		error = access ( path_str  , F_OK && how  );
		if ( error )
		{
			
			perror ( strerror ( error) ) ;
			assert ( BSTR_ERR != bdestroy ( path_bs ) );
			path_bs = NULL;
			
		}
		
	}
	
	/* En caso de encontrar una dirección , esta es reemplazada en bs */
	result = path_bs != NULL ;
	if (result)
	{
	
		/* En caso de perder el commando , puede ser a causa de destruir
		 * bs , ver el funcionamiento de bconcat () */
		assert ( BSTR_ERR != bassign ( bs , path_bs ) );
		
	}
	
	/* Poscondición */
	assert ( result == (path_bs != NULL ) ) ;
	
	path_bs = NULL;
	path_str = NULL;
	bdestroy (aux_bs);
	aux_bs = NULL;
	
	return result;
}


	

