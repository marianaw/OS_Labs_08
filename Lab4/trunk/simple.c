/* $Date: 2008-10-31 09:14:09 -0200 (Fri, 31 Oct 2008) $ */
/* $Revision: 682 $ */

#include "simple.h"
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <assert.h>
#include "inode.h"
#include <glib.h> /* para tener ghash */

/*                      */
/* FS simple en memoria */
/*                      */

/* Completar */
#define INVREP(self) ((self)!=NULL && self->answer==42)

/* El FS simple debería ser solo un mapa nombre -> inodo */
struct simple_s
{
	GHashTable * fs;
};


/*
 * Constructores y Destructores
 */

simple *simple_create(void)
{
	simple * result = NULL;
	GHashTable * dirs=NULL;
	
	result = calloc(1,sizeof(*result));
	if(result != NULL) {
		dirs = g_hash_table_new (g_str_hash, g_str_equal);
		result->fs=dirs;
		assert(INVREP(result));
	}
	return result;
}

void simple_destroy(simple *self)
{
	assert(INVREP(self));
	
	g_hash_table_destroy (self->fs);
	
	free(self);
	self =NULL;
}

/*
 * Operaciones Básicas
 */

int simple_getattr(simple *self, const char *path, struct stat *buf)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL && buf!=NULL);
	
	/* inicializar la estructura stat: */
	buf -> st_mode = S_IFREG | S_IFDIR;
	buf -> st_nlink = 1;
	buf->st_uid= getuid();
	buf->st_gid =getgid();
	buf->st_size = (1ULL << 32); /* por las dudas */
	buf-> st_blocks = /* no sé */ ;
	buf->st_atime = buf->st_mtime = buf->st_ctime = time(NULL);
	
	result = ENOENT;
	return result;
}

int simple_mknod(simple *self, const char *path, mode_t mode)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);
	
	/* no debe ser un fichero ya existente */
	if (! S_ISREG (mode))
		result = mknode (path, mode, dev);
	
	/* inserto el nuevo nodo en la ghash: */
	g_hash_table_insert (self->fs, path, &path); /* &path intenta ser el nodo :OS */
	
	return result;
}

int simple_unlink(simple *self, const char *path)
{
	int result = 0;
	gboolean b=true;
	
	assert(INVREP(self));
	assert(path!=NULL);

	/* debería ver si está en la tabla hash: */
	if (g_hash_table_lookup_extended (self->fs, path, NULL, NULL)){
		b= g_hash_table_remove (self->fs, path);
		if (b==false)
			return -1;
		result = unlink (path); /* ¿?¿? */
	}

	result=0;
	return result;
}

int simple_rename(simple *self, const char *from, const char *to)
{
	int result = 0;
	assert(INVREP(self));
	assert(from!=NULL && to!=NULL);	
	
	if (g_hash_table_lookup_extended (self->fs, path, NULL, NULL))
		g_hash_table_replace  (self->fs, new_name, &name); /* "&name" intenta ser el valor asociado a "name" */
	
	result=0;
	return result;
}

int simple_chmod(simple *self, const char *path, mode_t mode)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);	
	
	result = chmod (path, mode);
	if (result == -1)
		return -errno;

	result=0;
	return result;
}

/* falta esta: */
int simple_chown(simple *self,const char *path, uid_t uid, gid_t gid)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);	
	
	result = 0;
	return result;
}

int simple_read(simple *self, const char *path,
	      char *buffer, size_t size, off_t offset)
{
	int result = 0;
	inodo * valor=NULL;
	assert(INVREP(self));
	assert(path!=NULL);
	assert(buffer!=NULL);

	valor = g_hash_table_lookup (self-> fs, path);
	if (valor !=NULL){
		if (size + offset <= valor->metadata->st_size){
			memcpy (buffer, &(valor-> data [offset]), size);
			result=size;
		}
		else{
			memcpy (buffer, &( valor->data[offset]),(valor->metadata->st_size - offset));
			result = valor->metadata->st_size - offset;
		}
	}
	else
		return 0;

	return result;
}

int simple_write(simple *self, const char *path,
	       const char *buffer, size_t size, off_t offset)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);
	assert(buffer!=NULL);
	
	
	
	result = size;	
	return result;
}

int simple_utimens(simple *self, const char *path, const struct timespec tv[2])
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);
	
	result = 0;
	return result;
}

/*
 * Operaciones sobre Directorios
 */

int simple_mkdir(simple *self, const char *path, mode_t mode)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);

	result = 0;
	return result;
}

int simple_rmdir(simple *self, const char *path)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);
	
	result = 0;	
	return result;
}


int simple_readdir(simple *self, const char *path, void *buf,
		     fuse_fill_dir_t filler)
{
	int result = 0;
	assert(INVREP(self));
	assert(path!=NULL);
	assert(buf!=NULL);

	result = 0;
	return result;
}
