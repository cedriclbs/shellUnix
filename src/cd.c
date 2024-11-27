#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/cd.h"

int cmd_cd(char **args) {
    static char prev_dir[1024] = ""; // Répertoire précédent
    char current_dir[1024]; // Répertoire actuel avant le changement
    char temp_dir[1024]; // Variable temporaire pour échanger les répertoires
    char *path_h;

    // Sauvegarder le répertoire actuel avant de changer
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("cd");
        return 1; // Retourner 1 si getcwd échoue
    }

    // "cd" -> Aller au répertoire HOME
    if (args[1] == NULL) {
        path_h = getenv("HOME");
        if (path_h == NULL) {
            perror("cd");
            return 1; // Retourner 1 si l'accès au répertoire HOME échoue
        }
        if (chdir(path_h) != 0) {
            perror("cd");
            return 1; // Retourner 1 si chdir échoue
        }
    }
    // "cd -" -> Aller au répertoire précédent
    else if (strcmp(args[1], "-") == 0) {
        if (strlen(prev_dir) == 0) {
            fprintf(stderr, "cd: No previous directory\n");
            return 1; // Retourner 1 si il n'y a pas de répertoire précédent
        }
        // Sauvegarder le répertoire actuel temporairement
        strncpy(temp_dir, current_dir, sizeof(temp_dir) - 1);
        temp_dir[sizeof(temp_dir) - 1] = '\0';

        // Changer vers le répertoire précédent
        if (chdir(prev_dir) != 0) {
            perror("cd");
            return 1; // Retourner 1 si chdir échoue
        }

        // Mettre à jour prev_dir
        strncpy(prev_dir, temp_dir, sizeof(prev_dir) - 1);
        prev_dir[sizeof(prev_dir) - 1] = '\0';
    }
    // "cd arg" -> Changer vers args[1]
    else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return 1; // Retourner 1 si chdir échoue
        }
    }

    // Mettre à jour prev_dir après un changement réussi
    strncpy(prev_dir, current_dir, sizeof(prev_dir) - 1);
    prev_dir[sizeof(prev_dir) - 1] = '\0';

    return 0; // Retourner 0 si tout se passe bien
}
