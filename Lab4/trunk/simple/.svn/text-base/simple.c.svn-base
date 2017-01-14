/* $Date: 2008-10-31 09:14:09 -0200 (Fri, 31 Oct 2008) $ */
/* $Revision: 682 $ */

#include "simple.h"
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h> /* para tener getuid y getgid */
#include <sys/stat.h>

#include <assert.h>
#include "inode.h"
#include <glib.h> 	/* para tener hash table y otras funciones ,
					 * luego buscare las minimas posibles */
#include "bstring/bstrlib.h" /* para hacerme la vida mas facil en readdir */

/*                      */
/* FS simple en memoria */
/*                      */

/* Completar */
#define INVREP(self) ((self)!=NULL && self->table!=NULL)

/* El FS simple debería ser solo un mapa nombre -> inodo */
struct simple_s
{
	GHashTable * table;
};

/*
 * Funciones Privadas 
 */

/* Un Wrapper para inode_destroy ( se me hizo mas facil asi) */
static void value_destroy ( void * value );

/* retorna GList de los archivos del directorio, NULL de no haber.
 * el valor retornado debe ser liberado por el usuario. */
static GList * get_dir_elems ( GHashTable * table, const char * dir );

/* Chequea que path sea un elemento de dir */
static gboolean dir_child ( const char * path , const char * dir );

/* retorna char * result la dirección relativa de path respecto de dir.
 * el valor retornado debe ser liberado por el usuario. */
static char * relative_path ( const char * path , const char * dir );


/*
 * Constructores y Destructores
 */

simple *simple_create(void)
{
	/* Inicialización de Variables */
	
	simple *result = NULL;
	inode  *archive = NULL;
	
	result = calloc(1,sizeof(*result));

	if(result != NULL) {

		/*
		 * Creo la hash table , la cual tendra sus propias funciones
		 * de limpieza de clave y valor.
		 */
		
		result->table = g_hash_table_new_full (	g_str_hash , g_str_equal ,
							g_free , value_destroy );

		if (result->table == NULL)
		{
			free (result);
			result = NULL;
		}

		assert(INVREP(result));

		/*
		 * Creación e inicialización el inodo raiz , donde esta montado SimpleFS, e
		 * inicializacion de los valores de metadata.
		 */
		
		archive = inode_create();
		assert (archive!= NULL);
	
		archive->metadata.st_dev 		=0;			/* dispositivo */
		archive->metadata.st_ino 		=0;      	/* inodo */
		archive->metadata.st_mode = S_IFDIR; /* protección */
		archive->metadata.st_nlink 		= 2;		/* número de enlaces físicos */
		archive->metadata.st_uid 		=getuid();  /* ID del usuario propietario */
		archive->metadata.st_gid 		=getgid();  /* ID del grupo propietario */ 
		archive->metadata.st_blksize	=0;
		archive->metadata.st_rdev 		=0;     	/* tipo dispositivo (si es
													 * dispositivo inodo) */
		archive->metadata.st_size 		=0;     	/* tamaño total, en bytes */
		archive->metadata.st_blocks 	=0;   		/* número de bloques asignados,
													 * igual a size */
		time(&(archive->metadata.st_atime));
		time(&(archive->metadata.st_mtime)); 
		time(&(archive->metadata.st_ctime));
		
		g_hash_table_insert ( result-> table , g_strdup("/") , archive);

		archive = NULL;

	}

	return result;
}

void simple_destroy(simple *self)
{
	/* Precondiciones */
	assert(INVREP(self));
	
	g_hash_table_destroy (self->table);

	free(self);
	self = NULL;

}

/*
 * Operaciones Básicas
 */

int simple_getattr(simple *self, const char *path, struct stat *stbuf)
{
	/* Inicialización de Variables */
	int result = 0;
	inode * archive = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL && stbuf!=NULL);

	
	/* busco los atributos en la tabla */
	archive = g_hash_table_lookup (self->table, path);

	if( archive == NULL)
	{
		result = ENOENT;
	}
	else
		/* Copio los atributos a stbuf */
		memcpy (stbuf, &(archive->metadata) , sizeof(struct stat));
		
	archive = NULL;

	return result;
}

int simple_mknod(simple *self, const char *path, mode_t mode)
{
	/* Inicialización de Variables */
	int result = 0;
	inode * archive = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);

	/* Creo el Inodo y seteo sus metadatos */
	archive = inode_create ();
	assert (archive!= NULL);
	
	archive->metadata.st_dev 		=0;			/* dispositivo */
	archive->metadata.st_ino 		=0;      	/* inodo */
	archive->metadata.st_mode 		= mode; 	/* protección */
	
	switch (mode)
	{
		case S_IFDIR:
			archive->metadata.st_nlink 	= 2;	/* número de enlaces físicos */
			break;
		default :
			archive->metadata.st_nlink 	= 1;
			break;  
	}
	archive->metadata.st_uid 		=getuid();  /* ID del usuario propietario */
	archive->metadata.st_gid 		=getgid();  /* ID del grupo propietario */ 
	archive->metadata.st_blksize	=1;
	archive->metadata.st_rdev 		=0;     /* tipo dispositivo (si es
							   				 * dispositivo inodo) */
	archive->metadata.st_size 		=0;     /* tamaño total, en bytes */
	archive->metadata.st_blocks 	=0;		/* número de bloques asignados */	
	time(&(archive->metadata.st_atime));	/* tiempo de acceso */
	time(&(archive->metadata.st_mtime)); 	/* tiempo de modificación */
	time(&(archive->metadata.st_ctime));  	/* tiempo de cambios de metadata */

	g_hash_table_insert( self->table , g_strdup(path), archive);

	archive = NULL;
	
	return result;
}

int simple_unlink(simple *self, const char *path)
{
	/* Inicialización de Variables */
	int result = 0;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);

	/* Eliminación del archivo */
	if (!g_hash_table_remove (self->table, path))
		result = -1;

	result = 0;
	return result;
}

int simple_rename(simple *self, const char *from, const char *to)
{
	/* Inicialización de Variables */
	int result = 0;
	inode * old_archive = NULL; 
	inode * new_archive = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(from!=NULL && to!=NULL);


	/* Todavia no pude hacer funcionar lookup_extended , esto es temporal */	
	old_archive= g_hash_table_lookup( self->table, from );
	assert ( old_archive != NULL);
	
	new_archive = calloc ( 1 , sizeof(struct stat));
	assert ( new_archive != NULL);
	memcpy (new_archive, old_archive , sizeof(struct stat));
	
	
	g_hash_table_insert ( self->table , g_strdup(to) ,  new_archive  );
	g_hash_table_remove ( self->table , from );

	old_archive = NULL;
	new_archive = NULL;
	
	return result;
}

int simple_chmod(simple *self, const char *path, mode_t mode)
{
	/* Inicialización de Variables */
	int result = 0;
	inode * archive = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);	
	
	archive = g_hash_table_lookup (self->table,path);
	assert(archive !=NULL);
	
	/* Actualizio la metadata */
	archive->metadata.st_mode = mode;
	time(&(archive->metadata.st_ctime));
	
	archive = NULL;
	
	result = 0;

	return result;
}

int simple_chown(simple *self,const char *path, uid_t uid, gid_t gid)
{
	/* Inicialización de Variables */
	int result = 0;
	inode * archive = NULL;

	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);
	
	archive = g_hash_table_lookup (self->table,path);
	assert(archive !=NULL);
	
	/* Actualizio la metadata */
	archive->metadata.st_uid = uid;
	archive->metadata.st_gid = gid;
	time(&(archive->metadata.st_ctime));
	
	archive = NULL;

	result = 0;

	return result;
}

int simple_read(simple *self, const char *path,
	      char *buffer, size_t size, off_t offset)
{
	/* Inicialización de Variables */
	unsigned int cantidad = 0;
	inode * archive=NULL;

	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);
	assert(buffer!=NULL);

	
	/* Busco el archivo en la hash table y chequeo que el offset
	 * sea correcto */
	archive = g_hash_table_lookup (self-> table, path);
	assert(archive !=NULL);

	/* Solo se lee si hay datos para tal accion */
	if ((archive->metadata.st_size - offset) >=0)
	{
		/* Se copia tanto como se pueda leer a buffer del usuario.
		 * Si no se llega a satisfacer el pedido , se copia solo lo que hay.
		 */
		cantidad = MIN(size,archive->metadata.st_size - offset);
		memcpy (buffer, &(archive-> data [offset]), cantidad);
	}

	/* Actualizo la metadata (deberia hacerlo aun cuando no leyo?) */
	time(&(archive->metadata.st_atime));
	
	archive = NULL;
	
	return cantidad;
}

int simple_write(simple *self, const char *path,
	       const char *buffer, size_t size, off_t offset)
{
	/* Inicialización de Variables */
	unsigned int cantidad = 0;
	inode * archive=NULL;
	unsigned int l_writed = 0 ;
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);
	assert(buffer!=NULL);

	/* Busco el archivo en la hash table y chequeo que el offset sea correcto */
	archive = g_hash_table_lookup (self-> table, path);
	assert (archive != NULL);
	cantidad = MAX (archive->metadata.st_size, offset + size);
	
	/* Solo se escribo si el offset es valido */
	if (offset >=0)
	{
		/* Se guarda lo que el usuario envio para escribir.
		 * Se realoja un tamaño que acapare toda la informacion adicional.
		 */
		archive-> data  = realloc ( archive-> data , cantidad);
		memmove(&(archive-> data [offset]) , buffer , size);
		l_writed = size;
	}
	
    /* Actualizio la metadata (deberia hacerlo aun cuando no escribio? */
	archive->metadata.st_size = cantidad;
	time(&(archive->metadata.st_atime));
	time(&(archive->metadata.st_mtime));
	
	archive = NULL;
	
	return l_writed;
}

int simple_utimens(simple *self, const char *path, const struct timespec tv[2])
{
	/* Inicialización de Variables */
	int result = 0;
	inode * archive = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);

	
	archive = g_hash_table_lookup (self-> table, path);
	assert (archive != NULL);
	
	/* Actualizio la metadata */
	archive->metadata.st_atime = (time_t)tv[0].tv_sec;
	archive->metadata.st_mtime = (time_t)tv[1].tv_sec;
	time((&archive->metadata.st_ctime));
	
	archive = NULL;
	
	result = 0;
	return result;
}

/*
 * Operaciones sobre Directorios
 */

int simple_mkdir(simple *self, const char *path, mode_t mode)
{
	/* Inicialización de Variables */
	int result = 0;
	inode * archive = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);

	/* Creo y seteo el directorio */
	archive = inode_create();
	assert (archive!= NULL);

	archive->data = NULL;
	
	archive->metadata.st_dev 		=0;			/* dispositivo */
	archive->metadata.st_ino 		=0;      	/* inodo */
	archive->metadata.st_mode = S_IFDIR ; 			/* protección */
	archive->metadata.st_nlink 		= 1;		/* número de enlaces físicos */
	archive->metadata.st_uid 		=getuid();  /* ID del usuario propietario */
	archive->metadata.st_gid 		=getgid();  /* ID del grupo propietario */ 
	archive->metadata.st_blksize	=0;
	archive->metadata.st_rdev 		=0;     	/* tipo dispositivo (si es
												 * dispositivo inodo) */
	archive->metadata.st_size 		=0;     	/* tamaño total, en bytes */
	archive->metadata.st_blocks 	=0;   		/* número de bloques asignados,
												 * igual a size */
	time(&(archive->metadata.st_atime));
	time(&(archive->metadata.st_mtime)); 
	time(&(archive->metadata.st_ctime));

	/* Inserto el directorio */
	g_hash_table_insert ( self-> table , g_strdup(path) , archive);

	archive = NULL;

	result = 0;
	return result;
}

int simple_rmdir(simple *self, const char *path)
{
	/* Inicialización de Variables */
	GList * elems = NULL;

	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);

	/* Busco que exita el directorio */
	if (g_hash_table_lookup ( self->table , path) == NULL)
	{
		errno = ENOENT;
		return 1;
	}

	/* Chequeo que no existan elementos en el directorio */
	elems = get_dir_elems (self->table , path);
	if ( elems != NULL )
	{
		g_list_free( elems );
		errno = ENOTEMPTY;
		return 1;
	}

	/* Eliminación del archivo */
	if (!g_hash_table_remove (self->table, path))
	{
		return 1;
	}

	return 0;
}


int simple_readdir(simple *self, const char *path, void *buf,
		     fuse_fill_dir_t filler)
{
	/* Inicialización de Variables */
	GList * elems = NULL;
	char * elem = NULL;
	inode * i_elem = NULL;
	
	/* Precondiciones */
	assert(INVREP(self));
	assert(path!=NULL);
	assert(buf!=NULL);

	/* Fillers basicos y bonitos */
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	/* Obtengo los elementos en el directorio */
	elems = get_dir_elems (self->table , path);

	/* Agrego cada elemento al buffer */
	while ( elems != NULL)
	{
		/* Introdusco los elementos del directorio ,en forma relativa ,
		 * dentro del buffer y los remuevo de la lista.
		 */
		elem = g_list_nth_data (g_list_first (elems) , 0 );
		i_elem = g_hash_table_lookup ( self->table , elem );
		elem = relative_path ( elem , path);
		
		/* Lo agrego solamente si pertene al directorio directamente */
		if ( elem != NULL )
		{
			filler(buf, elem ,&(i_elem->metadata) , 0);
			
			/* elimina el primer elemento de la lista */
			elems = g_list_delete_link (elems , elems );
		}
	}

	/* Desapunto y destruye las variables auxiliares */
	elem = NULL; i_elem = NULL;
	
	return 0;
}

/*
 * Funciones Privadas 
 */

static GList * get_dir_elems ( GHashTable * table, const char * dir )
{
	/* Orden : length (self->table) */
	/* Inicialización de variables */
	GList * elem = NULL;
	GList * result = NULL;
	unsigned int l_table = 0;
	unsigned int i = 0;

	/* En caso de no haber que o donde buscar ,devuelvo NULL */
	if ( table == NULL)
	{
		errno = EBADF;
		return NULL;
	}

	/* Busco que exita el directorio */
	if ( g_hash_table_lookup ( table , dir) == NULL)
	{
		errno = ENOENT;
		return NULL;
	}
	
	/* Cargo todas las direcciones que hay en la Tabla Hash */
	result = g_hash_table_get_keys(table);
	l_table = g_list_length(result);
	
	/* Recorro la lista , descartando los que no son child .
	 * Orden : l_table  */
	for (i = l_table ; i>0; i--)
	{
		/* Elimino elem si no pertenece al directorio
		 * sino lo vuelvo a concatenar atras */ 
		elem = g_list_first (result);
		if ( !dir_child ( g_list_nth_data (elem, 0) , dir ) )
		{
			/* elimina el primer elemento de la lista */
			result = g_list_delete_link (result , result );
		}
		else
		{
			/* concateno el elemento al final de la lista */
			result = g_list_remove_link ( result , result );
			result = g_list_concat ( result , elem );
		}
	}
	
	/* Desapunto y destruye las variables auxiliares */
	elem = NULL;

	return result;
}

static gboolean dir_child ( const char * path , const char * dir )
{
	/* Inicialización de variables */
	char * elem = NULL;
	bstring b_elem = NULL;
	unsigned int  l_elem = 0;
	int pos = 0 ;

	/* Precondiciones */
	assert ( path != NULL );
	assert ( dir != NULL );

	/* Chequeo la dirección de path relativa a dir exista*/
	elem = relative_path ( path , dir );
	if ( elem == NULL)
		return FALSE;

	/* Convierto a bstring y obtengo el tamaño */
	b_elem = bfromcstr ( elem );
	l_elem = blength ( b_elem );
	
	/* Chequeo si el elemento es un directorio o archivo child */
	pos = bstrchr (b_elem , '/');

	/* Desapunto y destruyo las variables auxiliares, quizas deba reorganizar */
	assert ( bdestroy (b_elem) != BSTR_ERR );
	free ( elem ); elem = NULL;
	
	/* Obtengo cuantos elementos hay luego */
	return (pos == BSTR_ERR );
	
}

static char * relative_path ( const char * path , const char * dir)
{
	/* Inicialización de variables */
	bstring b_path = NULL;
	bstring b_dir = NULL;
	char * result = NULL;
	unsigned int l_dir = 0;
	int error = 0;

	/* Precondiciones */
	assert ( path != NULL );
	assert ( dir != NULL );
	
	/* Convierto path a bstring */
	b_path = bfromcstr ( path );
	
	/* Convierto dir a bstring y obtengo su largo */
	b_dir = bfromcstr ( dir );
	l_dir = blength ( b_dir );

	/* Chequeo que b_dir se encuentre al principio de b_path */
	if ( 0 == binstr(  b_path , 0 ,b_dir))
	{
		/* Obtengo el path relativo con '/' al principio */
		if (l_dir > 1)
		{
			error =	bdelete(b_path , 0 , l_dir );
			assert( BSTR_ERR != error  );
		}

		/* Elimino la barra que quedo */
		if (0 == bstrchr (b_path , '/') )
		{
			error =	bdelete(b_path , 0 , 1);
			assert( BSTR_ERR != error  );

			/* Obtengo el resultado */
			if ( blength (b_path) > 0 )
			{
				result = g_strdup( bdata (b_path) );
			}
			
		}
	}

	/* Desapunto y destruyo las variables auxiliares */
	error = bdestroy(b_path); assert (BSTR_ERR != error );
	error = bdestroy(b_dir); assert (BSTR_ERR != error );

	return result;

}

static void value_destroy ( void * value )
{
	inode_destroy ( (inode *) value);
}
