#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "timeval.h"


int main (void){
	
	/* inicialización de variables: */
	/*Será un stream al archivo /dev/fifo*/
	FILE * result=NULL;
	int i=0, j=0, repetir = 0;
	char * bloque=NULL;
	size_t size=0;
	int close=0;
	int err_time1=0, err_time2=0, err_timesubtract=0;
	/*Todas las mediciones que se vallan efectuando se van sumando y
	  almacenando en este vector, del siguiente modo: la i-ésima ubicación 
	  contiene la suma de los tiempos tomados para transferir un bloque de 2^i
	  bytes. Finalmente, el vector será empleado para almacenar la tasa promedio
	  de transferencia de cada tamaño de bloque*/
	double  * tiempos = NULL;
	/*Los emplearemos para guardar el tiempo de inicio, fin y tiempo transcurrido
	  de cada prueba*/
	struct timeval * start=NULL;
	struct timeval * end=NULL;
	struct timeval * res=NULL;

	/*Pedimos memoria para el vector de mediciones e inicializamos cada posición
	  en 0*/
	tiempos = (double*)calloc(17,sizeof(double));
	memset((void*)tiempos,0,17);
	
	/*Pedimos memoria para los struct timeval que vamos a emplear*/
	start=(struct timeval*)calloc(1,sizeof(struct timeval));
	end=(struct timeval*)calloc(1,sizeof(struct timeval)); 
	res=(struct timeval*)calloc(1,sizeof(struct timeval)); 
	/* escritura: */
	for(repetir=1; repetir<=1000;repetir++)
	{
		for (j=0; j<=16; j++)
		{
			/*Los bloques van a tener tamaños crecientes en potencias de 2*/
			i= 1<<j;
			/*Pedimos i bytes para el bloque*/
			bloque= (char *) calloc (i,sizeof (char));
			assert (bloque!=NULL);
			/*Tomamos el tiempo de inicio de la prueba*/
			err_time1=gettimeofday (start, NULL);
			/* abrir /dev/fifo para escritura */
			result=fopen ("/dev/fifo","r+");
			/*Escribimos el bloque en el archivo /dev/fifo*/
			size=fwrite ( (void *) bloque, i, 1, result);
			/* cerrar archivo: */
			close=fclose (result);
			/*Tomamos el tiempo de finalización de la prueba*/
			err_time2=gettimeofday (end, NULL);
			/*Corremos el chequeo de errores a esta sección del código para que no
			  interfieran con los tiempos que estamos midiendo*/
			assert (result !=NULL);
			if (close != 0)
			perror((char*)strerror(close));
			/*Chequeo de errores: size debería ser 1, pues es esa la cantidad de 
			  elementos de i bytes que hemos intentado escribir en la llamda fwrite.
			  A su vez, fflush debería haber devuelto 0 para indicar que no ha 
			  ocurrido error alguno*/
			if (size != 1 )
			{
				printf ("error \n");
				return 1;
			}
			else 
			{
					/*Tomamos el tiempo transcurrido*/
					err_timesubtract= timeval_subtract (res, end, start);
					/*El resultado no debería ser negativo*/
					if (err_timesubtract == 0)
					{
						
						printf("%f\n",tiempos[j]);
						if(j == 16)
							  printf("\n--------------------------\n");
						tiempos[j] = tiempos[j] + (double)(res -> tv_usec);
					}
					else
					{
						printf ("error en timeval_subtract o gettimeofday\n");
						return 1;
					}
			}
		}
	}
	printf("\n------------Tasa de transferencia-------\n");
	/*Calculamos un tiempo promedio, la tasa de transferencia y mostramos el
	  resultado*/
	for(i=0;i<=16;i++)
	{
		 /*Calculamos el tiempo promedio*/
		 tiempos[i] = tiempos[i]/1000.0;
		 /*Obtenemos la tasa de transferencia en Kb/s*/
		 tiempos[i] = ((1<<i)*1000000.0)/(1024.0*tiempos[i]);
		 printf("%f\n",tiempos[i]);
	}
	
	return 0;
}
