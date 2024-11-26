#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/cd.h"

int cmd_cd(char **args) {
    static char prev_dir[1024] = "";
    char current_dir[1024];
    char *path_h;

    // "cd" -> répertoire HOME
    if (args[1] == NULL) {
        path_h = getenv("HOME");
        if (path_h == NULL) {
            perror("cd");
            return 1;  // Retourner 1 si l'accès au répertoire HOME échoue
        } else if (chdir(path_h) != 0) {
            perror("cd");
            return 1;  // Retourner 1 si chdir échoue
        }
    }
    // "cd -" -> répertoire précédent
    else if (strcmp(args[1], "-") == 0) {
        if (strlen(prev_dir) == 0) {
            fprintf(stderr, "cd: No previous directory\n");
            return 1;  // Retourner 1 si il n'y a pas de répertoire précédent
        }
        if (chdir(prev_dir) != 0) {
            perror("cd");
            return 1;  // Retourner 1 si chdir échoue
        }
        printf("%s\n", prev_dir);
    }
    // "cd arg" -> changer de répertoire vers args[1]
    else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return 1;  // Retourner 1 si chdir échoue
        }
    }

    // Mettre à jour prev_dir pour cd -
    if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
        strncpy(prev_dir, current_dir, sizeof(prev_dir) - 1);
    } else {
        perror("cd");
        return 1;  // Retourner 1 si getcwd échoue
    }

    return 0;  // Retourner 0 si tout se passe bien
}
