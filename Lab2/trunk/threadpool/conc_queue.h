/*
 * Copyright (C) 2008 Carlos S. Bederián
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or any
 * later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* $Rev: 530 $ */
#ifndef CONC_QUEUE_H_
#define CONC_QUEUE_H_

/*! \file conc_queue.h
 * Cola concurrente
 *
 * Cola de lectura y escritura concurrente tomada de
 * Nonblocking Algorithms and Preemption-Safe Locking
 * on Multiprogrammed Shared Memory Multiprocessors
 * http://www.cs.rochester.edu/u/scott/papers/1998_JPDC_nonblocking.pdf
 * Fig.4, pag.10
 */

//! Puntero opaco a la estructura de la cola
typedef struct _conc_queue_t *conc_queue_t;

//! Constructor
/*! Devuelve una cola vacia.
 *
 *  \return cola con 0 elementos o NULL y ENOMEM si no hay memoria
 */
conc_queue_t conc_queue_create(void);

//! Destructor
/*! Libera la cola (no nula) y devuelve NULL 
 *
 *  \param q	cola no nula
 *  \return la nueva cola (NULL)
 */
conc_queue_t conc_queue_destroy(conc_queue_t q);

//! Enqueue
/*! Agrega un elemento al final de la cola y devuelve la nueva estructura.
 *
 *  \param q	cola no nula
 *  \param elem	el elemento a agregar
 *  \return	la nueva cola. Si no hay memoria, se devuelve la misma cola y ENOMEM
 */
conc_queue_t conc_queue_enqueue(conc_queue_t q, void *elem);

//! Dequeue
/*! Toma el primer elemento de la cola, lo pone en el espacio apuntado por
 *  elem y devuelve la nueva estructura. Si la cola esta vacia se escribe
 *  NULL en elem y EAGAIN en errno.
 *
 *  \param q	cola no nula
 *  \param elem	puntero a la memoria donde se escribe el elemento tomado
 *  \return	la nueva cola
 */
conc_queue_t conc_queue_dequeue(conc_queue_t q, void **elem);

//! Map
/*! Reemplaza cada elemento e de la cola por el resultado de func(e) y devuelve
 *  la nueva cola. Durante el proceso se bloquean los enqueues y dequeues.
 *
 *  \param q	cola no nula
 *  \param func	funcion de void* en void* que se aplica a los elementos
 *  \return	la nueva cola
 */
conc_queue_t conc_queue_map(conc_queue_t q, void *(*func)(void *));

//! Count
/*! Devuelve la cantidad de elementos de la cola. Durante el proceso se
 *  bloquean los enqueues y dequeues.
 *
 *  \param q	cola no nula
 *  \return	la cantidad de elementos de la cola
 */
unsigned int conc_queue_count(conc_queue_t q);

#endif /* CONC_QUEUE_H_ */
