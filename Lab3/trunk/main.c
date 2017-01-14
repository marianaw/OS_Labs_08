#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>		/* registrado de dispositivos */
#include <linux/types.h> 	/* define el tipo bool */

#include "debug.h"
#include "fifo_ops.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Controlador para dispositivo alternativo /dev/fifo");

/* Nombre del Dispositivo */
static const char *NAME = "fifo-0.1";

/* Tamaño del buffer */
static int size_b = 65536;
module_param(size_b, int, S_IRUGO);
MODULE_PARM_DESC ( size_b, " tamaño del dispositivo fifo ; default 1");

/* Major del dispositivo */
static int mayor_b = 250;
module_param(mayor_b, int, S_IRUGO);
MODULE_PARM_DESC ( mayor_b, " major del dispositivo fifo ; default 253");

static int fifo_init(void)
{
	int error = 0;
	
	error = register_chrdev(mayor_b, NAME, &mod_fifo_fops);
	
	switch (error)
	{
		case 0:
			dprintk("\n%s : Carga completada satisfactoriamente.\n", NAME);
			fifo_create(size_b);
		break;

		case -EBUSY:
			dprintk( "\n%s: El Mayor %d con el que desea registrar el dispositivo"
			"'%s' ya ocupado\n",NAME,mayor_b, NAME );
			error = -1;
		break;

		case -EINVAL:
			dprintk( "\n%s: El Mayor %d con el que desea registrar el dispositivo"
			"'%s' no es valido \n",NAME,mayor_b, NAME );
			error = -1;
		break;

		default:
			dprintk("\n%s: No se pudo registrar el dispositivo '%s' con"
			" major: %d.\n", NAME, NAME, mayor_b);
			error = -1;
		break;
	}
	
	return error;
}

static void fifo_exit(void)
{
	fifo_destroy();
	
	unregister_chrdev(mayor_b , NAME);
	dprintk("\n%s: Descarga completada satisfactoriamente.\n", NAME);
}

module_init(fifo_init);
module_exit(fifo_exit);

