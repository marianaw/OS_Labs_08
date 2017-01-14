#include "command.h"
#include <assert.h>
#include <stdlib.h>
#include <glib/gqueue.h>


/*---------------------------Scommand-------------------------------*/


struct scommand_s 
{
	
	bool	is_builtin;	/* True cuando es comando interno */
	GQueue *arg;		/* Lista de Argumentos */ 
	bstring out;		/* Redirección de la salida */
	bstring in;			/* Redirección de la entrada */
};

	/*Constructor */

scommand *scommand_new (void)
{
	
	scommand* result=NULL;
	
	result = calloc (1,sizeof (struct scommand_s) );	
	assert (result != NULL);
	
	result->is_builtin  = false;
	result->arg			= g_queue_new ();
	result->out			= NULL;
	result->in			= NULL;
	
	/* Poscondiciones */
	assert (	result							!= NULL	&& 
				scommand_is_empty(result)				&& 
				scommand_get_redir_in(result)	== NULL	&&
				scommand_get_redir_out(result)  == NULL	);
	
	return result;
	
}
 
	/* Destructor */

void scommand_destroy (scommand *self)
{
	
	/* Precondiciones */
	assert (self		!= NULL	&&
			self->arg   != NULL );
	
	g_queue_clear(self->arg );	
	g_queue_free (self->arg );
	
	/* Se apunta a NULL para el evitar Dangling Pointers */
	self->arg = NULL;
	
	/* El caso de que lo redirecciones de entrada y salida 
	 * no fueron destruidos exteriormente , se liberaran en
	 * este punto */
	if (self->out   != NULL)
		bdestroy (self->out	);
	if (self->in	!= NULL)
		bdestroy (self->in  );
	
	/* Se apuntan a NULL para el evitar Dangling Pointers */
	self->out   = NULL;
	self->in	= NULL;
	
	free(self);
	self = NULL;
	
	/* Poscondiciones */
	assert ( self == NULL );

}

	/* Modificadores */

void scommand_push_back (scommand *self, bstring argument)
{
	
	/* Precondiciones */
	assert (self != NULL		&&  
			argument != NULL	);
	g_queue_push_tail (self->arg ,argument);
	
	/* Poscondiciones */
	assert ( !g_queue_is_empty (self->arg));
	
}


void scommand_push_front (scommand *self, bstring argument)
{
	
	/* Precondiciones */
	assert (self		!= NULL	&&  
			argument	!= NULL	);
	
	g_queue_push_head (self->arg ,argument);
	
	/* Poscondiciones */
	assert ( !g_queue_is_empty (self->arg));
	
}


void scommand_pop_front (scommand *self)
{
	
	bstring aux = NULL;
	
	/* Precondiciones */
	assert (self != NULL					&&  
			!g_queue_is_empty (self->arg)   );
	
	aux = g_queue_pop_head (self->arg);
	
}


void scommand_set_builtin (scommand *self, bool builtin)
{
	
	/* Precondiciones */
	assert (self != NULL);

	self->is_builtin = builtin;

}

void scommand_set_redir_in (scommand *self, bstring filename)
{
	
	/* Precondiciones */
	assert (self != NULL);
	
	self->in = filename;
	
}

void scommand_set_redir_out (scommand *self, bstring filename)
{
	
	/* Precondiciones */
	assert (self != NULL);
	
	self->out = filename;
	
}

	/* Proyectores */

bool scommand_is_empty (const scommand *self)
{
	
	/* Precondiciones */
	assert (self		!=NULL  &&
			self->arg   !=NULL  );
		
	return g_queue_is_empty (self->arg);

}

unsigned int scommand_length (const scommand *self)
{

	/* Precondiciones */
	assert (self		!=NULL  &&
			self->arg   !=NULL  );
	
	/* Poscondiciones */
	assert ( (  g_queue_get_length (self->arg) == 0)	== 
				g_queue_is_empty (self->arg)			); 
	
	return g_queue_get_length (self->arg);

}

bstring scommand_front (const scommand *self)
{
	
	bstring result = NULL;
	
	/* Precondiciones */
	assert ( self !=NULL && !scommand_is_empty(self) );
	
	result  = g_queue_peek_head ( self->arg );
	
	/* Poscondiciones */
	assert ( result!=NULL );
		
	return result;

}

bool scommand_get_builtin (const scommand *self)
{
	
	/* Precondiciones */
	assert(self!=NULL);
	
	return self->is_builtin;
	
}

bstring scommand_get_redir_in (const scommand *self)
{
	
	/* Precondiciones */
	assert(self!=NULL);
	
	return self->in;

}

bstring scommand_get_redir_out (const scommand *self)
{
	
	/* Precondiciones */
	assert(self!=NULL);
	
	return self->out;
	
}

bstring scommand_to_string (const scommand *self)
{
	
	bstring result  = NULL;
	unsigned int n  = 1;
	
	/* Precondiciones */
	assert(self!=NULL);
	
	/* En caso de ser vacio , result = NULL */
	if (!scommand_is_empty (self))
	{
		
		result= bstrcpy(g_queue_peek_nth(self->arg,0));
		n=1;
		
		while(n < scommand_length(self))
		{
			bcatcstr (result," ");		
			bconcat (result,g_queue_peek_nth(self->arg,n));
			n++;
		}
		
		if(scommand_get_redir_out(self) != NULL)
		{
			bcatcstr (result," > ");
			bconcat (result,self->out);
		}
		
		if(scommand_get_redir_in(self) != NULL)
		{
			bcatcstr (result," < ");
			bconcat (result,self->in);
		}
		
	}
	
	/* Poscondiciones */
	assert(scommand_is_empty(self)				|| 
		   scommand_get_redir_in(self)==NULL	|| 
		   scommand_get_redir_in(self)==NULL	|| 
		   blength(result)>0					);
	
	return result;	
	
}

/*---------------------------Pipeline-------------------------------*/


struct pipeline_s
{
	GQueue *commands;   /* Lista de commandos */
	bool	wait;		/* True cuando hay que esperar */
};

	/* Constructor */

pipeline *pipeline_new (void)
{
	
	pipeline *result = NULL;
	
	result=calloc(1,sizeof(struct pipeline_s));
	assert (result != NULL);
	
	result->commands	= g_queue_new();
	result->wait		= true;
	
	/* Poscondiciones */
	assert(result != NULL				&& 
		   pipeline_is_empty(result)	&& 
		   pipeline_get_wait(result)	);
	
	return result;
	
}

	/* Destructor */

void pipeline_destroy (pipeline *self)
{
	
	
	/* Precondiciones */
	assert (self			!= NULL	&&
			self->commands  != NULL );

	
	g_queue_clear(self->commands );	
	g_queue_free (self->commands );
	
	/* Se apunta a NULL para el evitar Dangling Pointers */
	self->commands = NULL;

	free(self);
	self = NULL;
	
	/* Poscondiciones */
	assert ( self == NULL );
	
}

	/* Modificadores */

void pipeline_push_back (pipeline *self, scommand *sc)
{
	
	/* Precondiciones */
	assert(self !=NULL  && 
		   sc   !=NULL  );
	
	g_queue_push_tail(self->commands,sc);
	
	/* Poscondiciones */
	assert(!pipeline_is_empty(self));
	
}

void pipeline_pop_front (pipeline *self)
{
	
	/* Precondiciones */
	assert(self!=NULL				&& 
		   !pipeline_is_empty(self) );
	
	g_queue_pop_head(self -> commands);

}

void pipeline_set_wait (pipeline *self, const bool w)
{
	
	/* Precondiciones */
	assert(self!=NULL);
	
	self -> wait = w;
	
}

	/* Proyectores */

bool pipeline_is_empty (const pipeline *self)
{
	
	/* Precondiciones */
	assert(self != NULL);
	
	return g_queue_is_empty (self->commands);

}

unsigned int pipeline_length (const pipeline *self)
{
	
	/* Precondiciones */
	assert(self != NULL);
	
	return g_queue_get_length(self -> commands);

}

scommand *pipeline_front (const pipeline *self)
{
	
	scommand *result = NULL;
	
	/* Precondiciones */
	assert(self!=NULL				&& 
		   !pipeline_is_empty(self) );
	
	result = g_queue_peek_head(self -> commands);
	
	/* Poscondiciones */
	assert(result != NULL);
	
	return result;
	
}

bool pipeline_get_wait (const pipeline *self)
{
	
	/* Precondiciones */
	assert(self != NULL);
	
	return self -> wait;
	
}

bstring pipeline_to_string (const pipeline *self)
{
	
	bstring result		= NULL; /* Resultado */
	bstring transf_aux  = NULL;	/* Puntero auxiliar */
	unsigned int n		= 1;	/* Contador */
	
	/* Precondiciones */
	assert(self != NULL);
	
	/* En caso de ser vacio , result = NULL */
	if ( !pipeline_is_empty ( self) )
	{
		
		/* Se convierte los scommand obtenido a bstring´s y se
		 * almacenan */
		result = scommand_to_string (g_queue_peek_nth(self->commands,0));
	
		while(n<pipeline_length(self))
		{
			
			bcatcstr (result," | ");
			transf_aux = scommand_to_string (g_queue_peek_nth(self->commands,n));
			bconcat	 (result,transf_aux);
			bdestroy (transf_aux);
			transf_aux = NULL;
			n++;
			
		}
		
		/* Revisa si hay que esperar o no */
		if ( !pipeline_get_wait ( self))
			bcatcstr (result," &");
		
	}
	
	/* Poscondiciones */	
	assert(pipeline_is_empty(self)  || 
		   pipeline_get_wait(self)  || 
		   blength(result)>0		);
	
	return result;
	
}
