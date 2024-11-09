#ifndef BUILTINS_H
#define BUILTINS_H

int cmd_cd(char **args);
// Permet la redirection vers un chemin, ou vers le répertoire précédent, ou vers HOME.

int cmd_exit(char **args);
// Termine le processus fsh avec comme valeur VAL

#endif
