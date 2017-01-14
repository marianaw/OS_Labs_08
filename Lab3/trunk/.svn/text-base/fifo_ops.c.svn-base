#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/wait.h>			/* cola de espera */
#include <asm/uaccess.h>

#define ASSERT(p)	BUG_ON(!(p))
#define MALLOC(s, u)	kmalloc((s)*(u), GFP_KERNEL)
#define FREE(p)		kfree(p)

#include "debug.h"
#include "fifo_ops.h"

/* Funciones de dispositivo */

static int mod_fifo_open(struct inode *ip, struct file *fp)
{
	return 0;
}


static ssize_t mod_fifo_read(struct file *fp, char *buf, size_t count, loff_t *offset)
{
	/* Inicialización de variables */
	char fbuf;
	unsigned long error = 0;
	int cant_bytes = 0;
	 dprintk("\ndevfifo: mod_fifo_read - count = %d\n",(int)count);
	/* Copio datos al buffer del usuario hasta completar lo pedido */
	while(count - cant_bytes > 0 && !error)
	{
		if(!fifo_dequeue( &fbuf))
		{
			dprintk("\ndevfifo: readed_bytes = %d\n" , cant_bytes + 1);
		 	error = copy_to_user (buf + cant_bytes, &fbuf, 1);
			/*Agrego esto para asegurarnos de que siempre
			  al terminar el procedimiento, cant_bytes tenga la cantidad de bytes
			  que efectivamente se han leído*/
			if(!error)
			cant_bytes = cant_bytes + 1; 
		}
		else
			error = 1 ; 
	}
	dprintk("\n devfifo leídos : %d",cant_bytes);
	return cant_bytes;
}


static ssize_t mod_fifo_write(struct file *fp, const char *buf, size_t count, loff_t *offset)
{
	/* Inicialización de variables */
	char fbuf;
	bool queue = false;
	int cant_bytes = 0;
	 
	dprintk("\ndevfifo: mod_fifo_write - count = %d\n",(int)count);
	/* Copio datos al buffer del kernel hasta completar lo pedido */
	while(count - cant_bytes > 0 && !queue )
	{
		queue = true;
		
		/*Pedimos memoria para almacenar el byte de información
		fbuf = (void*)MALLOC(1,sizeof(char));*/
		
		/*Traemos el byte de información desde el espacio de usuario*/
		if (!copy_from_user (&fbuf , buf + cant_bytes  , 1))
		{	
			/*Lo almacenamos en nuestro fifo*/	
			if (!fifo_enqueue(fbuf))
			{
				dprintk("\ndevfifo: writed_bytes = %d\n" , cant_bytes+1 );
				cant_bytes = cant_bytes + 1;
				queue = false;	
			}
		}

		/* Destruimos esa memoria, ya que ya se copio al buffer 
		FREE (fbuf);
		fbuf = NULL;*/
	}
	dprintk("\n devfifo escritos : %d",cant_bytes);
	return cant_bytes;
}


static int mod_fifo_release(struct inode *ip, struct file *fp)
{
	return 0;
}


/**
 * La estructura que contiene punteros a cada una de las operaciones que se
 * pueden realizar sobre /dev/fifo. Esta es la única variable que exporta el
 * módulo. Así, podremos cambiar hasta los nombres de las funciones que
 * implementan cada operación y se preservará la abstracción.
 */


struct file_operations mod_fifo_fops = {
	.owner	=	THIS_MODULE,
	.open	=	mod_fifo_open,
	.read	=	mod_fifo_read,
	.write	=	mod_fifo_write,
	.release=	mod_fifo_release
};
