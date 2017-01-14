/*
* Copyright (C) 2008 Natalia B. Bidart
* 
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
* 
* You should have received a copy of the GNU General Public License along
* with this library; if not, see <http://www.gnu.org/licenses/>.
*/

/* $Rev: 530 $ */
#ifndef __UTILS_H
#define __UTILS_H

#ifdef __DEBUG
#include <stdio.h>
#define debug(X,...) printf(__FILE__ ": %s: " X, __func__, ##__VA_ARGS__)
#else
#define debug(X,...)
#endif

#endif /* __UTILS_H */
