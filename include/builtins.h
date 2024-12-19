#ifndef BUILTINS_H
#define BUILTINS_H

#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "ftype.h"
#include "for.h"

int execute_builtin(char **args, int argc,int val);

#endif
