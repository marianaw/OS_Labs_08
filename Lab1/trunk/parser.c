#include "parser.h"
#include "bstring/bstrlib.h"
#include "lexer/lexer.h"
#include "builtin.h"
#include <assert.h>
/*
 	Verifica la validez del comando considerando validarComienzo(l) y que no
	contenga caracteres inválidos.
	REQUIRES: l != NULL
 */
static bstring comandoValido(Lexer *l, bool cadena);
/*
 	Modularización del procedimiento para reconocer un caracter '\n' y
 finalizar el ciclo de parseo.
 REQUIRES: l != NULL
 */
static bool fin(Lexer *l);

/*
  	En caso de que se produzca un error en el proceso de parseo (mala sintáxis)
  se debe liberar los elementos almacenados en memoria dinámica del scommand.
  REQUIRES : s != NULL
  ENSURES : s == NULL
 */

static void limpiarScommand(scommand *s);
/*
  	En caso de que se produzca un error en el proceso de parseo (mala sintáxis)
  se debe liberar los elementos almacenados en memoria dinámica del pipeline.
  REQUIRES : p != NULL
  ENSURES : p == NULL
 */
static void limpiarPipeline(pipeline *p);
/*
  	En caso de que se produzca un error en el proceso de parseo (mala sintáxis)
	liberamos los tads pipeline y scommand. Llamamos los respectivos procedimientos
	desde aquí.
	REQUIRES: p != NULL && s != NULL
 */
static void terminarConError(pipeline *p, scommand *s);

pipeline* parsear(FILE *stream)
{
	/*
	  Requerimos de estas banderas para señalar cada estado del proceso 
	  de parseo
	  */
	bool comando = false;
	bool pipel = false;
	bool redir = false;
	bool seguir = false;
	bool arg = false;
	bool archivoIn = false;
	bool archivoOut = false;
	bool ampersand = false;
	pipeline *result = NULL;
	Lexer *l = NULL;
	scommand* cmd = NULL;
	bstring bstr = NULL;
	
	/* Precondición */
	assert(stream != NULL);
	
	l = lexer_new(stream);
	
	assert(l != NULL);
	
	cmd = scommand_new();
	result = pipeline_new();
	seguir = true;
	comando = true;
	/*
	  Descartamos los espacios en blanco o "tabs" que podrían estar al principio.
	  También chequeamos si no hay caracteres '\n', en cuyo caso no debemos
	  empezar el parseo.
	  */
	lexer_skip (l," \t");
	seguir = fin(l);
	while(!lexer_is_off(l) && seguir)
	{
		if(comando)
		{
			/*
			  En este momento deberíamos poder extraer un comando válido
			  */
			bstr = comandoValido(l,false);
			if(bstr == NULL)
			{
				/*
				  No había comando o no era válida su sintaxis. Corresponde terminar
				  con error el parseo.
				  */
				printf("\nFalta comando\n");
 				terminarConError(result,cmd);
				result = NULL;
				cmd = NULL;
				seguir = false;
			}
			else
			{
				/*
				  Guardamos el comando en la estructura scommand. Debería venir
				  un argumento. Ponemos la bandera arg en true
				  */
				comando = false;
				arg = true;
				scommand_push_back(cmd,bstr);
				scommand_set_builtin (cmd,is_builtin (bstr));
				/*
				  Siempre luego de extraer una cadena elminamos los espacios en
				  blanco y estudiamos la posibilidad de encontrarnos con un
				  caracter '\n'
				  */
				lexer_skip (l," \t");
				seguir = fin(l);
				
			}
		}
		if(arg && seguir)
		{
			 /*
			    Los argumentos no tiene prioridad, en el sentido de que de no poder
			  	 extraerse un argumento válido no es causa de error al momento del
			    análisis sintáctico.
			  */
			lexer_next_char(l,"\"");
			if(!lexer_is_off(l))
			{
				bstr = lexer_item(l);
				if(blength(bstr) > 0)
				{
					/*
					  Los argumentos se han ingreso entre comillas dobles. Debemos
					  aceptar cualquier caracter.
					  */
					bdestroy(bstr);
					bstr = comandoValido(l,true);
					if(bstr == NULL)
					{
						/*
						  El fallo se debió a falta de comillas de cierre
						  */
						seguir = false;
						printf("\nFaltan comillas de cierre\n");
						terminarConError(result,cmd);
						result = NULL;
						cmd = NULL;
					}
				}
				else
				{
					bstr = comandoValido(l,false);
				}
				if(bstr == NULL)
				{
					/*
					  Si no pudimos extraer argumentos, es de esperar que siga algún
					  caracter de redirección de E/S, o de tubería o de ejecución
					  en segundo plano. Estos casos se tratan en un solo bloque. 
					  Para entrar al mismo basta poner una de las 3 banderas que lo 
					  controlan en true. Optamos por redir
					  */
					arg = false;
					redir = true;
				}
				else
				{
					scommand_push_back(cmd,bstr);
				}
				lexer_skip (l," \t");
				seguir = fin(l);
			}
		}
		if((redir || pipel || ampersand) && seguir)
		{	
			lexer_next(l,redirIn redirOut pipe segundoPlano);
			if(!lexer_is_off(l))
			{
				bstr = lexer_item(l);
				if(blength(bstr) != 1)
				{
					printf("\nse esperaba |,< o >\n");
					terminarConError(result,cmd);
					result = NULL;
					cmd = NULL;
					seguir = false;
				}
				else
				{
					if(biseqcstr(bstr,pipe))
					{
						 /*
						  	De encontrarse un caracter de tubería es de esperar que 
						   le siga un comando. Ponemos comando en true para analisar
						   esto en la próxima iteración.
						  */
						comando = true;
						pipeline_push_back(result,cmd);
						cmd = scommand_new();
					}
					else
					{
						if(biseqcstr(bstr,redirIn))
						{
							/*
							  Encontramos un caracter de redirección de entrada. 
							  Esperamos un archivo.
							  */
							archivoIn = true;
						}
						else
						{
							if(biseqcstr(bstr,redirOut))
							{
								/*
							  		Encontramos un caracter de redirección de salida. 
							 	   Esperamos un archivo.
							  */
								archivoOut = true;
							}
							else
							{
								if(biseqcstr(bstr,segundoPlano))
								{
									/*
									  Al encontrar el caracter de ejecución en segundo
									  plano terminamos el parseo.
									  */
									pipeline_set_wait(result,false);
									seguir = false;
								}
							}
						}
					}
					lexer_skip (l," \t");
				 }
				 pipel = redir = ampersand = false;
				 bdestroy(bstr);
			}
		}
		 if((archivoIn || archivoOut)  && seguir)
		 {
			 bstr = comandoValido(l,false);
			 if(bstr == NULL)
			 {
				 /*
					Se esperaba la presencia de un archivo.
					*/
				 printf("\n Falta archivo de redirección de E/S\n");
 				 terminarConError(result,cmd);
				 result = NULL;
				 cmd = NULL;
				 seguir = false;
			 }
			 else
			 {
				 if(archivoIn)
				 {
					 scommand_set_redir_in (cmd,bstr);
					 archivoIn = false;
				 	 pipel = ampersand = redir = true;
				 }
				 else
				 {
					 scommand_set_redir_out (cmd,bstr);
				  	 archivoOut = false;
				    ampersand = redir = true;
				 }
				 lexer_skip (l," \t");
				 seguir = fin(l);
			 }
		 }
	}
	/*
	  Si el puntero cmd apunta a una estructura scommand que tiene al menos
	  un elemento corresponde almacenarlo en el tad.
	  */			
	if(cmd != NULL && !scommand_is_empty(cmd))
	{
		pipeline_push_back(result,cmd);
	}
	else
	{
		 if(cmd != NULL)
		 	scommand_destroy (cmd);
	}
	lexer_destroy(l);	
	/*Postcondiciones*/
	assert(result == NULL || (pipeline_is_empty (result) || 
		 pipeline_length (result) > 0));
	
	return result;
}

bool fin(Lexer *l)
{
	 bstring bstr = NULL;
	 /* Precondición */
	 assert(l != NULL);
	 lexer_next_char(l,"\n");
	 bstr = lexer_item(l);
	 return blength(bstr)!=1;
}
bstring comandoValido(Lexer *l, bool cadena)
{
	bstring aux = NULL;
	bool matar = false;
	 
	/* Precondición */
	assert(l != NULL && !lexer_is_off(l));
	
	/*
	  Si los argumentos están entre comillas (caso cadena = true) corresponde
	  aceptar cualquier caracter.
	  */
	if(cadena)
	{
		lexer_next_to(l,"\""); 
	}
	else
	{
		lexer_next_to(l,pipe redirIn redirOut segundoPlano BLANK);
	}
	if(!lexer_is_off(l))
	{
		aux = lexer_item(l);
		if(!cadena && blength(aux) == 0)
		{
			/*
			  Se esperaba un comando y no se pudo extraer ningún caracter válido
			  */
			matar = true;
		}
		else
		{
			if(cadena)
			{
				/*
				  Debería venir una comilla de cierre
				  */
				lexer_next(l,"\"");
				if(!lexer_is_off(l))
				{
					bstring bstr = lexer_item(l);
					if(blength(bstr) % 2 == 0)
					{
						 /*
						  	Se ha observado en el shell que debe haber una cantidad 
						   impar de comillas de cierre.
						  */
						printf("\nFaltan comillas\n");
						matar = true;
					}
					bdestroy(bstr);
				}
				else
				{
					/*
					  No se pudieron extraer comillas de cierre. Corresponde terminar
					  con error.
					  */
					matar = true;
				}
			}
		}
		if(matar)
		{
			bdestroy(aux);
			aux = NULL;
		}
	}
	return aux;
}

void limpiarPipeline(pipeline *p)
{
	scommand *aux = NULL;
	unsigned int n = 0;
	
	/* Precondición */
	assert(p != NULL);
	
	/*
	  El usuario del tad pipeline debe liberar todos los elementos almacenados
	  en memoria dinámica que podría contener el tad.
	  */
	while(n < pipeline_length(p))
	{
		aux = pipeline_front (p);
		limpiarScommand(aux);
		pipeline_pop_front(p);
		n++;
	}
	/*
	  Destruimos efectivamente la estructura pipeline
	  */
	pipeline_destroy (p);
	p = NULL;
	/*Postcondición*/
	assert(p == NULL);
}

void limpiarScommand(scommand *s)
{
	bstring aux = NULL;
	unsigned int n = 0;
	
	/* Precondición */
	assert(s != NULL);
	
	/*
	  El usuario del tad scommand debe liberar todos los elementos almacenados
	  en memoria dinámica que podría contener el tad.
	  */
	while(n < scommand_length(s))
	{
		aux = scommand_front (s);
		bdestroy(aux);
		scommand_pop_front(s);
		n++;
	}
	/*
	  Destruimos efectivamente la estructura scommand
	  */
	scommand_destroy (s);
	s = NULL;
	/*Postcondición*/
	assert(s == NULL);
}

void terminarConError(pipeline *p, scommand *s)
{
	assert(p != NULL && s != NULL);
	limpiarPipeline(p);
	limpiarScommand(s);
}
