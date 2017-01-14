/*
* Copyright (C) 2007 Natalia B. Bidart
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
#ifndef __TASK_H
#define __TASK_H

typedef void *arg_t;

typedef void *(*routine_t)(void *);

typedef struct _task_t *task_t;

/*  PRE: assert(a_routine != NULL) */
/* POST: assert(result != NULL) */
/* POST: assert(task_routine(result) == a_routine) */
/* POST: assert(task_arg(result) == an_arg) */
task_t task_create(routine_t a_routine, arg_t an_arg);

/*  PRE: assert(self != NULL) */
/* POST: assert(result != NULL) */
routine_t task_routine(task_t self);

/*  PRE: assert(self != NULL) */
arg_t task_arg(task_t self);

/*  PRE: assert(self != NULL) */
/* POST: assert(self == NULL) and all memory used by self is freed */
task_t task_destroy(task_t self);

#endif /* __TASK_H */
