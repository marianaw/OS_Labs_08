#ifndef PARSER_H
#define PARSER_H
#define pipe "|"
#define redirIn "<"
#define redirOut ">"
#define segundoPlano "&"
#define parserBlank "\t\r "

#
#include "command.h"
#include <stdio.h>
/*
	Comandos simples: 
	* dirección relativa del comando/nombre de comando argumentos
	* dirección absoluta del comando/nombre de comando argumentos
	* nombre de comando argumentos

	Observación:
	* dirección relativa puede comenzar con ./
	* dirección absoluta siempre comienza con /
	* nombre de comando no puede empezar con .,/. Tampoco puede contener /
 	* Preguntar por "\"
 	* nombre de comand no puede contener |
*/


/*
 	Construir un objeto pipeline de la información parseada accedida a través
 	del flujo stdin.
 	REQUIRES: stdin != NULL
 	ENSURES: result != NULL && (pipeline_is_empty (result) || 
		 pipeline_length (result) > 0)
 
 */
pipeline* parsear(FILE *stream);

#endif
