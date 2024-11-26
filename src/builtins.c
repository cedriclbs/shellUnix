#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/builtins.h"

int execute_builtin(char **args, int val) {
    if (args[0] == NULL) {
        // Pas de commande saisie
        return 1;
    }

    // Gestion des différentes commandes
    if (strcmp(args[0], "cd") == 0) {
        return cmd_cd(args);
    } else if (strcmp(args[0], "exit") == 0) {
        if(val == 1){
            cmd_exit(args);
            return val;
        }
        return cmd_exit(args);
    } else if (strcmp(args[0], "pwd") == 0) {
        return cmd_pwd();
    } else if (strcmp(args[0], "ftype") == 0) {
        return cmd_ftype(args);
    } else {
        return 0;
    }
}
