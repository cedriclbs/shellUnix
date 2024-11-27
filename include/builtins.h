#ifndef BUILTINS_H
#define BUILTINS_H

#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "ftype.h"
#include "for.h"

// Prototype pour exécuter une commande builtin
int execute_builtin(char **args, int val);

#endif
