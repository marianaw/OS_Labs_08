#include <stdio.h> /* printf */
#include <stdbool.h> /* bool, true, false */
#include <assert.h> /* assert */
#include <stdlib.h> /* calloc, free, strtoull */
#include "primality_checker.h" /* thread_pool_{t, create, destroy, execute_task} */

#include "thread_pool.h"
#include "task_queue.h"
#include "conc_queue.h"

#define POOL_SIZE 2
#define QUEUE_SI




int main (){
	task_queue_t tas;
	
	tas=NULL;
	
	task_queue_print (tas);
	
	return 0;
}

// /*
// void task_queue_print (task_queue_t q)
// {
// 	conc_queue_t con;
// 	/* precondición */ 
// 	assert(q != NULL);
// 
// 	con = conc_queue_map (q->tasks, (void *) task_queue_convertir());
// 	
// 	/* postcondición */
// 	assert(q != NULL);
// }
// 
// void * task_queue_convertir (void){
// 	/* Inicialización de Variables */
// 	task_t t;
// 	routine_t routine = NULL;
// 	
// 	/*precondición*/
// 	assert (t!=NULL);
// 	
// 	routine = task_routine(t);
// 	
// 	/*se realiza el print de la dirección de memoria*/
// 	printf ("%p\n", routine );
// 	
// 	routine = NULL;
// 	
// 	return ((void *)routine );
// }*/
