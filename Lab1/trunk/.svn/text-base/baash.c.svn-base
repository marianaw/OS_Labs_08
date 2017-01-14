#include "command.h"/* solo estara temporalmente */
#include "parser.h"
#include "path_transform.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

void cleanPipeline(pipeline *p);

/* Main viejo */
int main (void) {
	char* pwd = NULL;
	bool exe = true;
	pipeline* pl = NULL;
	/*Pedimos para pwd un tamaño correspondiento con un máximo que vamos a 
	  establecer, de 256 caracteres*/
	pwd = calloc(256,sizeof(char));
	/*Almacenamos el PWD*/
	pwd = strcpy(pwd,getenv("PWD"));
	while (exe) 
	{
		printf("\n%s", getenv ( "PS2" ));
		pl = parsear (stdin);
		if (pl == NULL) {
			printf("\n%s", getenv ( "PS2" ));
		}else if (path_transform (pl,pwd)) {
			exe =  execute (pl,pwd);
		}	
		cleanPipeline ( pl);
	}
	return 0;
}

void cleanPipeline(pipeline* p)
{
	
	/* Inicialización de Variables */
	scommand* aux = NULL;
	unsigned int i = 0 , j = 0; 
	bstring bstr = NULL;
	
	/* Precondición */
	assert(p != NULL);
	
	
	for (j = 0; j < pipeline_length(p) ;j++ )
	{
		aux = pipeline_front (p);
		for ( i = 0; i < scommand_length(aux) ;i++ )
		{
			bstr = scommand_front (aux);
			bdestroy(bstr);
			scommand_pop_front(aux);
		}
		pipeline_pop_front(p);
		scommand_destroy (aux);
		aux = NULL;
	}
	
	pipeline_destroy(p);
	p = NULL;
	
}

