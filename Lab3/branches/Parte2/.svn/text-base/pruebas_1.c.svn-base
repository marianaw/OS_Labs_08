#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#define TAM 10

int main (void){
	
	/* inicialización de variables: */
	FILE * result=NULL;
	int i=0, j=0;
	char * bloque=NULL;
	size_t size=0;
	int close=0;

	/* abrir /dev/fifo para escritura */
	result=fopen ("/dev/fifo","r+");
	assert (result !=NULL);
	
	/* escritura: */
	for (j=0; j<=16; j++){
		i= 1<<j;
		bloque= (char *) calloc (i,sizeof (char));
		assert (bloque!=NULL);
		printf("\nSe van a escribir %d bytes",i);
		size=fwrite ( (void *) bloque,i, 1, result);
		printf("\nSe escribieron %d bytes",size);
		 
		if (size != 1){
			printf("\nerror\n");
			return 1;
		}
	}
	
	/* cerrar archivo: */
	close=fclose (result);
	if (close != 0)
		perror((char*)strerror(close));
	
	return 0;
}
