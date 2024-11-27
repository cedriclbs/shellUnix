#include <stdlib.h>
#include <ctype.h>
#include <stdio.h> 
#include "../include/exit.h"

int cmd_exit(char **args, int val) {
    int ret = val;

    // Vérifie si un argument est passé
    if (args[1] != NULL) {
        // Vérifie si l'argument est un nombre entier valide
        for (int i = 0; args[1][i] != '\0'; i++) {
            if (!isdigit(args[1][i])) {
                fprintf(stderr, "exit: %s: argument numerique requis\n", args[1]);
                exit(255);
            }
        }
        ret = atoi(args[1]);
    }
    exit(ret);
}