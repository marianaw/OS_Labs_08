#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#define DEBUG
#include "debug.h"
#include "fifoops.h"

MODULE_AUTHOR("Edgardo Hames - Julio Bianco");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Controlador para dispositivo alternativo /dev/nulo");

static const int MAJOR = 249;
static const char *NAME = "nulo-0.3";

int nulo_init(void)
{
	int error = 0;
	
	error = register_chrdev(MAJOR, NAME, &mod_fifo_fops);
	if (error == 0) {
		dprintk("%s : Carga completada satisfactoriamente.\n", NAME);
	} else {
		dprintk("%s: No se pudo registrar el dispositivo '%s' con"
			" major: %d.\n", NAME, NAME, MAJOR);
		error = -1;
	}
	
	return error;
}

void nulo_exit(void)
{
	int error = 0;

	unregister_chrdev(MAJOR, NAME);
	dprintk("%s: Descarga completada satisfactoriamente.\n", NAME);
}

module_init(nulo_init);
module_exit(nulo_exit);
