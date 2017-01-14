#ifndef BUILTIN_H
#define BUILTIN_H
#include "bstring/bstrlib.h"
#include <stdbool.h>/*  para tener bool */
bool is_builtin (bstring bs);

bool exec_builtin (char ** argv, char *pwd);


#endif
