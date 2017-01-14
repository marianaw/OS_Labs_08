#include "builtin.h"
#include <unistd.h>/*   para la syscall chdir */
#include <assert.h> /*  para aserciones */
#include <stdlib.h>/*   para el exit */
#include <errno.h>  /* Agrega constantes simbolicas para manejo de errores */
#include <stdio.h> /* Para manejo de errores y otros */
#include "bstring/bstrlib.h"

/* Constantes representantes de Comandos internos */
#define _cd "cd"
#define _exit "exit"

void b_cd (char* path, char *pwd)
{
	/* Inicialización de Variables */
	int	 error   = 0 ; 
	bstring bstr_path = NULL;
	bstring bstr_absolute = NULL;
	
	/* No utilizo aserción en este caso por que si es nulo
	 * significa que no tengo que ir a ningun lado por que 
	 * hizo cd solo */
	if(path != NULL)
	{
		/* Convierto el path a bstring y lo concateno a la direccion
		 * donde se encuentra el prompt actualmente */
		bstr_path = bfromcstr ( path);
		/*bstr_absolute = bfromcstr ( getenv ( "PWD" ) );*/
		bstr_absolute = bfromcstr(pwd);
		/* Al path absolute le concatenamos '/' y la dirección relativa del
		  directorio al cual queremos cambiar*/
		assert ( BSTR_OK == bconchar (bstr_absolute,'/') && 
				  BSTR_OK == bconcat ( bstr_absolute , bstr_path ));
	
		printf ("buscando\n : %s" , bdata ( bstr_absolute ) );
		error = chdir (bdata ( bstr_absolute ) );
		
		if ( error != 0){
			perror ( strerror ( error) ) ;
		}
		
		/*Copiamos el actual directorio en pwd*/
		pwd = strncpy(pwd,bdata(bstr_absolute),strlen(bdata(bstr_absolute)));
		/* Destrucción de las Variables */
		assert ( BSTR_ERR != bdestroy (  bstr_absolute ) );
		bstr_absolute = NULL;
		assert ( BSTR_ERR != bdestroy (  bstr_path ) ) ;
		bstr_path = NULL;
	}
	
}

bool is_builtin (bstring bs)
{
	/* Inicialización de Variables */
	char* bs_str = NULL;
	bool  result = false;
	
	/* Precondición */
	assert (bs!= NULL);

	bs_str = bdata(bs);
	result = ( 0 == strcmp ( bs_str , _cd )|| 0 == strcmp ( bs_str , _exit) );

	bs_str = NULL;	
	return result;
		
}
bool exec_builtin (char ** argv, char* pwd)
{
	
	/* Precondición */
	assert ( argv != NULL );
	
	if ( 0 == strcmp ( _cd , argv[0]) )
	{   
		b_cd (argv[1], pwd);
		return true;
	}
	if ( 0 == strcmp ( _exit , argv[0]) )
	{   
		printf ("\nGood bye\n");
		exit(0);
		return false;
	}
	
	return false;

}
