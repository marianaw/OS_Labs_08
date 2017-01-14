#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "conc_queue.h"
#include "task_queue.h"


/*
	Definición del TAD opaco "task_queue". Se trata de una cola sincronizada.
	Implementamos el modelo productor-consumidor para controlar que no ocurran
   	buffers over/under runs. Para esto empleamos 2 semáforos: uno contabiliza
 	los lugares disponibles en la conc_queue, el otro contabiliza las tareas
 	en cola.
*/
struct _task_queue_t
{
	unsigned int queue_size;
	conc_queue_t tasks;
	sem_t llenos;
	sem_t vacios;
};


/*
	Esta función se encarga de imprimir las task que le pasamos
*/
void * task_queue_convertir (void * t);

task_queue_t task_queue_create(const unsigned int size)
{
	 /* inicializacion de variables */
	 int 		error 	= 0;
	 task_queue_t 	result 	= NULL;
	 
	 /* precondición */
	 assert(size > 0);
	 
	 result = (task_queue_t)calloc(1,sizeof(struct _task_queue_t));
	 assert(result != NULL);
	 
	 result -> queue_size = size;
	 /*
	   Construimos un objeto conc_queue.
	  */
	 result -> tasks = conc_queue_create();
	 assert(result -> tasks != NULL);
	 
	 /*
	  	Inicializamos los semáforos. El segundo argumento es 0 para que estos
	  	semáforos sean compartidos entre los threads.
	  */
	 error = sem_init (&result -> llenos,0,0);
	 assert( error == 0);
	 error = sem_init (&result -> vacios,0,result -> queue_size);
	 assert( error == 0);
	 
	
	 /* postcondición */
	 assert(result != NULL);
	 
	 return result;
}

task_queue_t task_queue_enqueue(task_queue_t q, task_t t)
{
	 /* precondición */
	 assert (q != NULL);
	 assert (t != NULL);
	 
	 /*
	   Decrementamos el semáforo contador de lugares disponibles.
	  */
	 sem_wait(&(q -> vacios));
	 q -> tasks = conc_queue_enqueue(q -> tasks,(void *)t);
	 /*
	  	Chequeamos que se haya encolado la tarea
	  */
	 assert(errno != ENOMEM);
	 /*
	   Incrementamos el semáforo contador de tareas en cola.
	  */
	 sem_post(&(q -> llenos)); 
	 
	 /* postcondición */
	 assert(q -> tasks != NULL);
	 return q;
}

task_queue_t task_queue_dequeue(task_queue_t q, task_t *t)
{
	 
	 /* precondición */
	 assert(q != NULL);
	 assert(*t == NULL);
	 
	 /* Vamos a extraer una tarea. Controlamos que esto sea posible, decrementando
	  * el semáforo contador de tareas en cola.*/
	 sem_wait(&(q -> llenos));
	 
	 q -> tasks = conc_queue_dequeue(q -> tasks,(void **)t);
	 
	 /* Incrementamos el semáforo contador de espacios disponibles. */
	 sem_post(&(q -> vacios));
	 
	 /* postcondición */
	 assert(q != NULL);
	 assert(*t != NULL);
	 return q;
}

task_queue_t task_queue_destroy(task_queue_t q)
{
	 /* Inicializacion de Variables */
	 int error = 0 ;

	 /* precondición */ 
	 assert(q != NULL);
	 
	 /* Destruimos la conc_queue.*/
	 q -> tasks = conc_queue_destroy(q -> tasks);
	 q -> tasks = NULL;
	 
	 /* Liberamos la memoria reservada para los semáforos de la task_queue.
	  * Chequeamos que no se devuelvan códigos de error. */
	 error = sem_destroy(&q -> llenos);
	 assert( error == 0);
	 error = sem_destroy(&q -> vacios);
	 assert( error == 0);
	 
	 free(q);
	 q = NULL;
	 
	 /* postcondición */
	 assert(q == NULL);
	 
	 return q;
}


void task_queue_print(task_queue_t q)
{
	/* precondición */ 
	assert(q != NULL);

	conc_queue_map (q->tasks,task_queue_convertir);
	
	/* postcondición */
	assert(q != NULL);
}

unsigned int task_queue_count(task_queue_t q)
{
	 unsigned int result = 0;
	 /* precondición */
	 assert(q != NULL);
	 
	 result = conc_queue_count(q -> tasks);

	 return result;
}

void * task_queue_convertir (void * t){
	/* Inicialización de Variables */
	routine_t routine = NULL;
	
	/*precondición*/
	assert (t!=NULL);
	
	routine = task_routine((task_t)t);
	
	/*se realiza el print de la dirección de memoria*/
	printf ("%p\n", (void *) routine );
	
	routine = NULL;
	
	return ((void *) t);
}
