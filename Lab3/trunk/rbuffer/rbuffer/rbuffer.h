#ifndef	__RBUFFER_H
#define	__RBUFFER_H

#ifdef __KERNEL__
#    include <linux/types.h> /* define el tipo bool */
#else
#    include <stdbool.h>
#endif
typedef char byte;

typedef struct _rbuffer_t* rbuffer_t;


/**
 * rbuffer_create - Crea un nuevo buffer circular.
 *
 * @size: Tamaño del buffer.
 *
 * Resultado: Un nuevo buffer circular en caso de éxito o NULL en caso de 
 * error.
 * 
 * */
extern rbuffer_t rbuffer_create(const unsigned int size);

/**
 * rbuffer_destroy - Destruye un buffer circular.
 *
 * @buffer:	El buffer circular a destruir.
 * 
 * */
extern void rbuffer_destroy(rbuffer_t buf);

/**
 * rbuffer_is_full - Indica si el buffer circular está lleno.
 *
 * @buf:	El buffer circular.
 *
 * Resultado: true si está lleno, false en caso contrario.
 * 
 * */
extern bool rbuffer_is_full(rbuffer_t buf);

/**
 * rbuffer_is_empty - Indica si el buffer circular está vacío.
 *
 * @buf:	El buffer circular.
 *
 * Resultado: true si está vacío, false en caso contrario.
 * 
 * */
extern bool rbuffer_is_empty(rbuffer_t buf);

/**
 * rbuffer_insert - Inserta un byte en el buffer circular.
 *
 * @buf:	El buffer circular.
 * @byte:	El byte a escribir en el buffer.
 *
 * Resultado: false si puede escribir, true si está lleno.
 *
 * */
extern bool rbuffer_insert(rbuffer_t buf, const byte data);

/**
 * rbuffer_remove - Lee un byte del buffer circular.
 *
 * @buf:	El buffer circular.
 * @byte:	Referencia al byte que contendrá el dato leído.
 *
 * Resultado: false si puede leer, true si está vacío.
 *
 * */
extern bool rbuffer_remove(rbuffer_t buf, byte *data);

#endif	/* __RBUFFER_H */

