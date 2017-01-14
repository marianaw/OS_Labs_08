#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define TAM 10

int main (void){
	
	/* inicialización de variables: */
	FILE * result=NULL;
	char *ptr = NULL;
	int ret = 1;
	/* abrir /dev/fifo para lectura */
	result=fopen ("/dev/fifo","r+");
	assert (result !=NULL);
	/*Pedimos un bloque de...¡65536 bytes!*/
	ptr=(char*)calloc(65536,sizeof(char));
	assert(ptr != NULL);
	/* lectura: */
	while(1)
	{
		ret = fread(ptr,1,65536,result);
		printf("\nSe han leído %d bytes");
		if(ret != 65536)
		{
			printf("\nerror\n");
			return 1;
		}
	}
	return 0;
}
