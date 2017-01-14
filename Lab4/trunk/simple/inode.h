/* $Date: 2008-10-31 09:14:09 -0200 (Fri, 31 Oct 2008) $ */
/* $Revision: 682 $ */

#ifndef __INODE_H
#define __INODE_H

#include <sys/stat.h>

typedef struct inode_s inode;

/* El inodo es un par (metadatos,datos) NO OPACO */
struct inode_s
{
	struct stat metadata;
	char *data;
};

/* 
 * Cuando un lenguaje implementa una tupla de manera razonable no tiene sentido
 * reimplementarla. "C" maneja bien la copia y las funciones proyectoras
 * sobre estructuras, o sea sobre tuplas con nombres en las dimensiones.
 */

/* Constructores y destructores */
inode *inode_create(void);
void inode_destroy(inode *self);

#endif /* __INODE_H */
