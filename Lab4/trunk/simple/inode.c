/* $Date: 2008-10-31 09:14:09 -0200 (Fri, 31 Oct 2008) $ */
/* $Revision: 682 $ */

#include "inode.h"
#include <malloc.h>
#include <sys/stat.h>
#include <assert.h>


/* Creación de un inodo */
inode *inode_create(void)
{
	inode *result = NULL;
	result = calloc(1, sizeof(*result));
	/* metadata está dentro de la estructura, luego se crea con el de arriba y todo a 0 */
	if (result!=NULL) {
		result->data = NULL;
	}
	return result;
}

/* Liberación de toda la memoria de un inodo */
void inode_destroy(inode *self)
{
	assert(self!=NULL);
	free(self->data);
	/* self->metadata está dentro de la estructura y se libera con el todo */
	free(self); self = NULL;
}
