#include <stdbool.h>			/* bool */
#include <pthread.h>			/* pthread_mutex_* */
#include <stdlib.h>			/* calloc, free */
#include <stdio.h>			/* perror */
#include <string.h>			/* strerror */
#include <assert.h>			/* assert */
#include <sys/errno.h>			/* manejo de errores */
#include <glib-2.0/glib/gqueue.h>	/* todas las funciones gqueue_* */
#include "conc_queue.h"			/* declaraciones */


/*
 * Cola de lectura y escritura concurrente basada en el paper:
 * Nonblocking Algorithms and Preemption-Safe Locking
 * on Multiprogrammed Shared Memory Multiprocessors
 * http://www.cs.rochester.edu/u/scott/papers/1998_JPDC_nonblocking.pdf
 * Fig.4, pag.10
 */

typedef unsigned int locks_i;

enum locks_i 
{ 
	Q_LOCK, 
	Q_UNLOCK
};

struct _conc_queue_t 
{
	pthread_mutexattr_t 	attr;
	pthread_mutex_t 	mutex;
	GQueue			*queue;
};

/* Funciones Internas */

/* Esta función se encarga del chequeo y la destrucción de mutex.
 * Si el mutex es NULL simplemente sera devuelto y si no lo es sera
 * destruido. En caso de que este bloqueado se mostrara un error y se abortara.
 * Esta funcion se desarrollo a causa de que en repetidas situaciones
 * tengo que destruir mutex.
 */
static void mutex_destroy ( pthread_mutex_t * mutex );


/* Esta función se encarga de realizar los locks y unlocks del mutex
 * que le es asignado. Si lock = Q_LOCK realizara un lock del mutex , en
 * en cambio si lock = Q_UNLOCK realizara un unlock del mutex.
 * En caso de falla se enviara el mensaje de error corespondiente y abortara.
 * Requires : mutex != NULL && (lock ==  Q_LOCK || lock ==  Q_LOCK )
 */
static void lock_manager ( pthread_mutex_t * mutex ,locks_i  lock );


/* Funciones Publicas */


/* Constructor */

/* Aclaración : Los manejos de errores dentro de esta función intentan ser 
 * lo mas robustos posible para poder devolver NULL en caso de falla, salvo
 * que se produsca alguna anomalia extraña en la creación de los mutexs.
 */
conc_queue_t conc_queue_create(void)
{
	/* Inicialización de Variables */
	conc_queue_t result 	= NULL;
	bool error_flag		= false;
	
	
	/* Creación del TAD thread_pool */
	if (!error_flag )
		result = calloc ( 1 , sizeof ( struct _conc_queue_t ) );
	error_flag = (result == NULL); 
	
	if (!error_flag )
	{
		/* Creamos el atributo para el mutex */
		pthread_mutexattr_init ( &(result->attr));
		pthread_mutexattr_settype ( &(result->attr) , PTHREAD_MUTEX_ERRORCHECK );
		
		/* Creamos el mutex */
		pthread_mutex_init ( &(result->mutex), &(result->attr) );
	}
	
	/* Creamos la cola */
	if (!error_flag )
		result->queue = g_queue_new ();
	error_flag = ( result->queue == NULL );
	
	
	/* Manejo de Errores */
	if ( error_flag )
	{
		errno = ENOMEM;
	
		/* Chequeo la con_queue */
		if ( result != NULL )
		{
			/* Destrucción de el mutex */
			mutex_destroy (&(result->mutex));
			
			/* Destrucción de el atributo de los mutex */
			pthread_mutexattr_destroy ( &(result->attr) );
			
			/* Chequeo y destrucción de la cola */
			if ( result->queue != NULL )
			{
				g_queue_free ( result->queue );
				result -> queue = NULL;
			}
			
			/* Destrucción de la Con_queue */
			free (result);
			result = NULL;
		}
	}
	
	/* Poscondición */
	assert 	(( error_flag && result == NULL ) || 
		( !error_flag && result != NULL ) );
	
	return result;
}


/* Destructor */

conc_queue_t conc_queue_destroy(conc_queue_t q)
{
	/* Precondición */
	assert ( q != NULL );
	
	/* Destrucción de el mutex */
	mutex_destroy (&(q->mutex));
	
	/* Destrucción de el atributo de los mutex */
	pthread_mutexattr_destroy ( &(q->attr) );
	
	/* Chequeo y destrucción de la cola */
	if ( q->queue != NULL )
	{
		g_queue_free ( q->queue );
		q -> queue = NULL;
	}
	
	/* Destrucción de la Con_queue */
	free (q);
	q = NULL;
	
	/* Poscondición */
	assert ( q == NULL );
	
	return q;
}


/* Modificadores */


conc_queue_t conc_queue_enqueue(conc_queue_t q, void *elem)
{
	/* Precondición */
	assert ( q != NULL );
	assert ( elem != NULL);

	 lock_manager ( &(q -> mutex) , Q_LOCK ); /*Inicio de la Sección Critica */
	
	
	/* manejo de errores */
	if ( g_queue_get_length ( q -> queue ) < G_MAXUINT )
		
		/* Se introduce elem en la con_queue */
		g_queue_push_tail ( q -> queue , elem );
	else
		errno = ENOSPC; /* "No hay espacio suficiente" */
	
	 lock_manager ( &(q -> mutex) , Q_UNLOCK ); /*Fin de la Sección Critica */
	
	/* Poscondición */
	assert ( q != NULL );
	
	return q;
}


conc_queue_t conc_queue_dequeue(conc_queue_t q, void **elem)
{
	/* Precondición */
	assert ( q != NULL );
	assert ( *elem == NULL );
	
	 
	lock_manager ( &(q -> mutex) , Q_LOCK ); /*Inicio de la Sección Critica */
	/* Se obtiene el elem de la con_queue */
	*elem = g_queue_pop_head ( q -> queue );
	
	/* Manejo de errores */
	if ( *elem == NULL )
	{
		errno = EAGAIN;
		assert ( *elem != NULL );
	}
	lock_manager ( &(q -> mutex) , Q_UNLOCK ); /*Fin de la Sección Critica */
	
	/* Poscondición */
	assert ( q != NULL );
	
	return q;
}


conc_queue_t conc_queue_map(conc_queue_t q, void *(*func)(void *))
{
	/* Precondición */
	assert ( q != NULL );
	assert ( func != NULL );
	
	/* Comienzo de la Seccion Critica */
	lock_manager ( &(q -> mutex) , Q_LOCK );
	
	g_queue_foreach ( q-> queue , (GFunc)func , NULL );
	
	lock_manager ( &(q -> mutex) , Q_UNLOCK );
	
	/* Poscondición */
	assert ( q != NULL );
	
	return q;
}

unsigned int conc_queue_count(conc_queue_t q)
{
	/* Precondición */
	assert ( q != NULL );
	
	return g_queue_get_length ( q -> queue );
}


/* Funciones Internas */

static void mutex_destroy ( pthread_mutex_t * mutex )
{
	/* Inicialización de Variables */
	int error = 0;
	
	/* Destrucción del mutex */
	if ( mutex != NULL)
		error = pthread_mutex_destroy ( mutex );
	
	/* Manejo de errores */
	if ( error != 0 )
	{
		perror ( strerror ( error ) );
	}
	
}


static void lock_manager ( pthread_mutex_t * mutex , locks_i lock )
{
	/* Inicialización de Variables */
	int error = 0;
	
	/* Precondición */
	assert ( mutex != NULL );
	switch (lock)
	{
		case Q_LOCK :
			error = pthread_mutex_lock ( mutex );
			break;
		case Q_UNLOCK :
			error = pthread_mutex_unlock ( mutex );
			break;
		default :
			/* Precondición */
			assert (false);
			break;
		
	}

	/* Manejo de errores */
	if ( error != 0 )
	{
		perror ( strerror ( error ) );
	}
	
}
