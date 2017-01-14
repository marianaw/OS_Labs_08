#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/module.h>


#define DEBUG

#include "debug.h"
#include "nuloops.h"


static int nulo_open(struct inode *ip, struct file *fp)
{
	return 0;
}

static ssize_t nulo_read(struct file *fp, char *buf, size_t count, loff_t *offset)
{
	dprintk("nulo: read %zd.\n", count);
	return 0;
}

static ssize_t nulo_write(struct file *fp, const char *buf, size_t count, loff_t *offset)
{
/**
 * Siempre se puede escribir en /dev/nulo y nunca nos quedaremos sin espacio.
 */
	dprintk("nulo: write %zd.\n", count);
	return count;
}


static int nulo_release(struct inode *ip, struct file *fp)
{
	return 0;
}

/**
 * La estructura que contiene punteros a cada una de las operaciones que se
 * pueden realizar sobre /dev/nulo. Esta es la única variable que exporta el
 * módulo. Así, podremos cambiar hasta los nombres de las funciones que
 * implementan cada operación y se preservará la abstracción.
 */

struct file_operations nulo_fops = {
	.owner	=	THIS_MODULE,
	.open	=	nulo_open,
	.read	=	nulo_read,
	.write	=	nulo_write,
	.release=	nulo_release
};
