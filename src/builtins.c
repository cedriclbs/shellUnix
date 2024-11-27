#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/builtins.h"
#include "../include/execute.h"

int execute_builtin(char **args, int val) {
    if (args[0] == NULL) {
        return val; // Pas de commande de saisie
    }

    if (strcmp(args[0], "cd") == 0) {
        return cmd_cd(args);
    } else if (strcmp(args[0], "exit") == 0) {
        return cmd_exit(args, val);
    } else if (strcmp(args[0], "pwd") == 0) {
        return cmd_pwd();
    } else if (strcmp(args[0], "ftype") == 0) {
        return cmd_ftype(args);
    } else if (strcmp(args[0], "for") == 0) {
        return cmd_for(args, val);
    } else {
        int result = execute_command_with_redirection(args);
        if (result == -1) {
            perror("Commande inconnue");
        }
        return result;
    }
}
