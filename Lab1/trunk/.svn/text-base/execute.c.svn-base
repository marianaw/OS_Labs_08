#include "execute.h"
#include <stdlib.h>	  /*  para asignación de memoria		*/
#include <stdio.h>	  /*  para manejo de errores y otros	*/
#include <unistd.h>	  /*  para las syscall's				*/
#include <assert.h>	  /*  para el manejo de aserciones		*/
#include <sys/wait.h>  /*  para wait							*/
#include <sys/types.h> /*  para pid_t						*/
#include <fcntl.h>     /*  para flags y permisos al llamar a open */
#include <sys/stat.h>  /*  Idem */
#include "builtin.h"   /*  Para la ejecución de comandos internos */
#include <stdbool.h>   /*  para tener bool					*/

char ** make_argv (scommand * sc);
/* Toma la gqueue de argumentos del tad scommand y devuelve un
 * puntero a la cadena de strings. 
 *
 * Returns : Lista de Argumentos de tipo Char *
 * Requires: sc!=NULL && !scommand_is_empty ( sc)
 */

void destroy_argv ( char** argv , int l );
/* Destruye la lista de argumentos.
 * Requires : argv != NULL;
 * Ensures :  argv == NULL;
 */

void redireccionar (scommand * com);
/* Efectúa los open, dup y close en caso de redirección
	requiere : com !=NULL */


char ** make_argv (scommand * sc){
	/* Inicialización de Variables */
	char ** argv=NULL;
	int i;
	bstring ar=NULL;
	
	/* Precondición */
	assert ( sc!=NULL );
	
	argv = (char **) calloc (scommand_length (sc)+1, sizeof (char *) );
	assert ( argv != NULL);
		
	for (i=0;i<scommand_length (sc); i++){
		
		ar= scommand_front (sc);
		assert ( ar != NULL);
		argv[i] = bstr2cstr ( ar , '\0' );/* ATENCION : leer el manual de bstring */
		assert ( argv[i] != NULL);
		
		scommand_pop_front (sc);
		scommand_push_back (sc, ar);
	}
	/*La función execv exige que el último elemento del vector de argumentos
	  sea un puntero a NULL*/
	argv[scommand_length (sc)]=NULL;
	ar = NULL;
	
	 /* Poscondición */
	assert ( sc != NULL				  &&
			 !scommand_is_empty ( sc)   );
	
	return argv;
}

void destroy_argv ( char** argv , int l )
{
	/* Inicialización de Variables */
	unsigned int i = 0 ;

	/* Precondición */
	assert ( argv != NULL);
	
	for ( i=0 ; i < l ; i++)
	{
		assert ( BSTR_OK == bcstrfree ( argv[i] ) );
		argv[i] = NULL;
	}
	
	free (argv);
	argv = NULL;
	
	/* Poscondición */
	assert ( argv == NULL );
	
}

void redireccionar (scommand * com)
{
	
	/* declaración de variables e inicialización */
	int fd = 0;
	int flags = 0;
	int permisos = 0;
	char *str = NULL;
	
	/* precondición */
	assert (com != NULL);
	
	/* si tiene redirección de entrada: */
	if (scommand_get_redir_in (com) != NULL)
	{
		
		/* inicializo las flags y los permisos */
		flags = O_RDONLY;
		permisos= S_IRWXU | S_IRUSR;
			
		str = bdata (scommand_get_redir_in (com));
		fd = open (str, flags, permisos);
		close (STDIN_FILENO);
	
		if (dup (fd)< 0)
		{
			perror ("falló dup");
		}
	}
		
	/* si tiene redirección de salida: */
	if (scommand_get_redir_out (com) != NULL)
	{
			
		/* inicializo flags y permisos: */
		flags = O_CREAT|O_WRONLY|O_TRUNC;
		permisos= S_IRWXU | S_IRUSR;
		str = bdata (scommand_get_redir_out (com));	
		fd = open (str, flags, permisos);
		close (STDOUT_FILENO);
			
		if (dup (fd)<0)
		{
			perror ("falló dup");
		}
	}
}

bool execute ( pipeline *pl, char* pwd )
{
	/* Inicialización de Variables */
	pid_t fork_id = 0;
	int** pipe_fds = NULL;
	char **args = NULL;
	char *command = NULL;
	scommand *cmd = NULL;
	int status = 0;
	int i = 0;
	int j = 0;
	/*int pipe_nuevo = 0;*/
	bstring bstr = NULL;
	
	/* Precondicion */
	assert ( pl != NULL				  &&
			 !pipeline_is_empty ( pl) );

	if(pipeline_length(pl) > 1)
	{
		/* Necesitamos pipeline_length(pl)-1 vectores de enteros */
		pipe_fds = (int**)calloc(pipeline_length(pl)-1,sizeof(int*));
		for(i = 0; i < pipeline_length(pl)-1;i++)
		{
			pipe_fds[i] = (int*)calloc(2,sizeof(int));
			if (pipe(pipe_fds[i]) < 0) 
			{
				perror("Problema al crear la tubería");
				exit(1);
			}
		}	
	}

	for(i = 0; i < pipeline_length(pl); i++)
	{
		cmd = pipeline_front(pl);
		bstr = scommand_front(cmd);
		command = bdata(bstr);
		args = make_argv(cmd);
		pipeline_push_back(pl,cmd);
		pipeline_pop_front(pl);
		/*De ser un comando interno y no estar en una estructura de pipes no
		  creamos un proceso hijo donde ejecutarlo, si no que en el contexto del
		  proceso padre llamamos a la función exec_builtin para efectuar el
		  comando*/
		if(scommand_get_builtin(cmd) && pipeline_length(pl) == 1)
		{
			return exec_builtin (args,pwd);
		}
		/*
		  
		  
		redireccionar (cmd);*/
		fork_id = fork();
		if (fork_id < 0) 
		{
			perror("Problema al crear un proceso hijo");
			exit(1);
		};
		if(fork_id == 0)
		{
			 redireccionar(cmd);
			if(pipeline_length(pl) != 1)
			{
				/*Tendremos que cerrar selectivamente las puntas de los pipes, 
				dependiendo de la ubicación del comando en el pipe*/ 
				if(i == 0)
				{
					/*Estamos en el proceso hijo. El comando a ejecutarse está en
					el extremo izquierdo del pipe.
					Cerramos la salida del pipe, no la 
					usamos. También cerramos la salida estándar */
					if(close(pipe_fds[0][0])<0 || close(STDOUT_FILENO)<0)
					{
						perror("Problema al intentar cerrar un stream");
						exit(1);
					}
					/*Ahora la salida estándar es la entrada del 
					pipe*/
					if(dup(pipe_fds[0][1])<0)
					{
						perror("Problema duplicando descriptores de archivos");
						exit(1);
					}
					for(j = 1;j<pipeline_length(pl) - 1;j++)
					{
						if(close(pipe_fds[j][0]) < 0 || close(pipe_fds[j][1]) < 0)
						{
							perror("Problema al intentar cerrar un stream");
							exit(1);	
						}
					}
				}
				else
				{
					if(i == pipeline_length(pl)-1)
					{
						/*Estamos en el proceso hijo. El comando a ejecutarse está en
			 			el extremo derecho del pipe.Cerramos la salida del pipe, 
						no la usamos. También cerramos la entrada estándar.*/
						if(close(pipe_fds[pipeline_length(pl)-2][1])<0 || close(STDIN_FILENO)<0)
						{
							perror("Problema al intentar cerrar un stream");
							exit(1);
						}
						/*Duplicamos salida del pipe. Es ahora 
						la entrada estándar del proceso hijo*/
						if(dup(pipe_fds[pipeline_length(pl)-2][0])<0)
						{
							perror("Problema duplicando descriptores de archivos");
							exit(1);
						}
						/*Cerramos las puntas de todos los pipes anteriores*/
						for(j = 0; j<pipeline_length(pl)-2;j++)
						{
							if(close(pipe_fds[j][0])<0 || close(pipe_fds[j][1])<0)
							{
								perror("Problema al intentar cerrar un stream");
								exit(1);
							}
						}
						
					}
					else
					{
						/*Estamos en el proceso hijo. El comando a ejecutarse está entre
						otros 2 comandos*/
						/*Cerramos la entrada estándar y salida estándar del proceso hijo*/
						if(close(STDIN_FILENO)<0 || close(STDOUT_FILENO)<0 ||
						   close(pipe_fds[i-1][1]) || close(pipe_fds[i][0])) 
						{
							perror("Problema al intentar cerrar un stream");
							exit(1);
						}
						/*Duplicamos la salida y la entrada de los pipes i-1 e i, respectivamente. 
						Notar que acá el orden en el que ejecutamos las llamadas al sistema 
						es importante.*/
						if(dup(pipe_fds[i-1][0])<0 || dup(pipe_fds[i][1])<0)
						{
							perror("Problema duplicando descriptores de archivos");
							exit(1);
						}
						for(j = 0;j < pipeline_length(pl) - 1; j++)
						{
							/*Cerramos las puntas de los pipes restantes*/
							if(j == i-1)
							{	
								j = i+1;
							}
							if(j < pipeline_length(pl)-1)
							{
								if(close(pipe_fds[j][0]) < 0 || close(pipe_fds[j][1]) < 0)
								{
									perror("Problema al intentar cerrar un stream");
									exit(1);	
								}
							}
						}
					}
				}
			}
			/*De ser un comando interno (caso is_builtin(bstr) = true) no lo
			debemos ejecutar efectuando la llamada a execv.*/
			if(!is_builtin (bstr))
			{
				execv (command,args);
			}
			else
			{
				exec_builtin (args,pwd);
			}
			printf("\nError en el comando %s : ",command);/*Si llegamos a este
			punto se considera que hubo error en la ejecución del comando*/
			perror(NULL);
			return false;
		}
		destroy_argv (args,scommand_length (cmd));
	}
	 
	for(i = 0; i < pipeline_length(pl)- 1; i++)
	{
		if(close(pipe_fds[i][0])<0 || close(pipe_fds[i][1])<0)
		{
			perror("Problema al intentar cerrar un stream");
			exit(1);
		}
	}
	
	if(pipeline_get_wait(pl))
	{
		/* Es necesario esperar por cada proceso hijo */
		for(i = 1; i<= pipeline_length(pl); i++)
		{
			wait(&status);
		}
	}
	
	/*Liberamos la memoria empleada para pipe_fds*/
	if(pipeline_length(pl) > 1)
	{
		for(i = 0;i < pipeline_length(pl)-1;i++)
		{
			free(pipe_fds[i]);
		}
		free(pipe_fds);
	}
	return true;
}
	
