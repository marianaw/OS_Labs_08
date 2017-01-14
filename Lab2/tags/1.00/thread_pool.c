#include <assert.h>	/* assert */
#include <stdlib.h>	/* calloc */
#include <pthread.h>	/* pthread_create , pthread_join , pthread_exit */
#include <stdio.h>	/* perror , strerror */
#include <errno.h>	/* Manejo de errores */
#include <stdbool.h>	/* Bool */
#include "task_queue.h" /* Todas las funciones task_queue_* */
#include "thread_pool.h"/* Declaraciones */

/*
 * Macro para no perder la legibilidad del código al intentar 
 * aumentar las llamadas a sched_yield.
 */
#define _Y yield()


struct _thread_pool_t
{
	unsigned int pool_tam;
	unsigned int queue_tam;
	pthread_t *trd;
	task_queue_t tasks;
};

/* Variables Globales */

/* Funciones Internas */

/* Esta funcion se encarga de buscar tasks en la task_queue 
 * y ejecutarlas. Esta funcion sera utilizada uniciamente
 * por los threads para que puedan buscar tasks a ejecutar.
 *
 * Requires : tasks != NULL */
void *thread_pool_task (void *tasks);

/* Esta funcion es solo un mod simple del sched_yield() ,
 * en el cual se realiza sched_yield de forma random.
 * Esta función fue tomada del ejemplo setBuffer.c
 */
void yield(void);


/* Funciones Publicas */

thread_pool_t thread_pool_create(const unsigned int pool_size, 
				 const unsigned int queue_size)
{
	/* Inicialización de Variables */
	thread_pool_t result	=NULL;
	unsigned int i		=0;
	int error		=0;
	
	/* Precondición */
	assert ( pool_size > 0 ); 
	assert ( queue_size > 0 );
	
	/* Creación del TAD thread_pool */
	result = (thread_pool_t) calloc (1 , sizeof(struct _thread_pool_t));
	assert( result != NULL);
		
	result-> pool_tam	= pool_size;
	result-> queue_tam   = queue_size;
		
	result-> tasks		= task_queue_create (queue_size);
	assert ( result-> tasks != NULL );
	
	result-> trd		= (pthread_t*)calloc(pool_size, sizeof(pthread_t));
	assert ( result-> trd != NULL );
	
	/* Creo los threads, a los cuales les asigno la funcion interna
	 * thread_pool_task para que busquen tareas y las ejecuten. */
	for (i=0;i<result->pool_tam;i++)
	{
		errno = pthread_create (&((result -> trd)[i]), NULL, 
					thread_pool_task,(void *)result -> tasks);
		
		assert ( !error );
	}
	
	/* Poscondición */
	assert ( result != NULL );
	
	return result;
}

void thread_pool_execute_task(thread_pool_t p, routine_t f, arg_t arg)
{
	/* Inicialización de Variables */
	task_t task	= NULL;
	
	/* Precondición */
	assert ( p != NULL ); 
	assert ( f != NULL );
	
	task= task_create (f,arg);
	assert ( task != NULL );
	
	p->tasks = task_queue_enqueue (p->tasks, task);
	
	/* Poscondición */
	assert ( p->tasks != NULL );
}

thread_pool_t thread_pool_destroy(thread_pool_t p)
{
	/* Inicialización de Variables */
	unsigned int i		= 0;
	task_t bomb_task	= NULL; /* bomba para matar threads */
	
	/* Precondición */
	assert(p != NULL);
	
	/* Creo un task pthread_exit() el cual voy a utilizar como mi bomba */
	bomb_task = task_create ( (routine_t) pthread_exit , NULL );
	assert (bomb_task != NULL);
	
	/* Envio una cantidad de bombas igual a la cantidad de threads */
	for(i = 0; i < p->pool_tam; i++)
	{
		p -> tasks = task_queue_enqueue (p -> tasks , bomb_task );
	}
	
	/* Espero a todos los thread a que se terminen de  */
	for(i = 0; i < p->pool_tam; i++)
	{
		pthread_join((p -> trd)[i],NULL);
	}		
	
	
	assert(task_queue_count (p -> tasks) == 0 );
	
	/* Destruimos Variables internas */
	bomb_task= task_destroy (bomb_task);
	assert (bomb_task == NULL);
	
	/* Destrucción del TAD thread_pool */
	p -> tasks = task_queue_destroy(p -> tasks);
	free (p -> trd);
	p->trd = NULL;
	free(p);
	p = NULL;
	
	 /* Poscondición */
	assert(p == NULL);
	
	return(p);
}

/* Funciones Internas */

void *thread_pool_task (void *tasks)
{
	/* Inicialización de Variables */
	task_queue_t	taskss	= (task_queue_t) tasks;
	task_t		task	=NULL; 
	routine_t 	routine	=NULL;
	arg_t 		arg	=NULL;
	
	while( true )
	{
		/* Precondición */
		if ( task != NULL )
			task= task_destroy (task);
		assert ( tasks != NULL );
		
		/* Obtengo el Thread a ejecutar */
		task_queue_print(taskss);_Y;
		taskss = task_queue_dequeue(taskss,&task);_Y;
		task_queue_print(taskss);_Y;
		
		/* Ejecuto la task y le entrego la cpu
		 * a otro thread. Luego destruyo la task*/
		routine = task_routine(task);_Y;
		arg = task_arg(task);_Y;
		routine(arg);_Y;

	}
	
	/* Para evitar punteros dangling */
	task	=NULL; 
	routine	=NULL;
	arg	=NULL;
}

void yield(void)
{
	if (rand()%2)
		sched_yield();
}

