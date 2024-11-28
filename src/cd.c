#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/cd.h"

/**
 * @brief Implémente la commande interne `cd` pour changer de répertoire.
 *
 * Cette fonction gère les différents cas de la commande `cd` :
 * - `cd` sans argument : bascule vers le répertoire HOME.
 * - `cd -` : bascule vers le dernier répertoire visité.
 * - `cd <path>` : bascule vers le répertoire spécifié dans `path`.
 *
 * En cas d'erreur (ex. répertoire introuvable), un message d'erreur est affiché,
 * et la fonction retourne un code d'erreur.
 *
 * @param args Tableau d'arguments de la commande. `args[0]` est la commande elle-même,
 *             et `args[1]` (optionnel) est le chemin ou l'option `-`.
 * @return Un entier représentant le code de retour :
 *         - 0 si l'opération réussit.
 *         - 1 si une erreur survient (ex. répertoire introuvable, accès refusé, etc.).
 */
int cmd_cd(char **args) {
    static char prev_dir[1024] = ""; // Répertoire précédent
    char current_dir[1024]; // Répertoire actuel avant le changement
    char temp_dir[1024]; // Variable temporaire pour échanger les répertoires
    char *path_h;

    // Sauvegarde le répertoire actuel avant de changer
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("cd");
        return 1; 
    }
    // "cd" -> va au répertoire HOME
    if (args[1] == NULL) {
        path_h = getenv("HOME");
        if (path_h == NULL) {
            perror("cd");
            return 1; 
        }
        if (chdir(path_h) != 0) {
            perror("cd");
            return 1; 
        }
    }
    // "cd -" -> va au répertoire précédent
    else if (strcmp(args[1], "-") == 0) {
        if (strlen(prev_dir) == 0) {
            fprintf(stderr, "cd: No previous directory\n");
            return 1; 
        }
        // Sauvegarde le répertoire actuel temporairement
        strncpy(temp_dir, current_dir, sizeof(temp_dir) - 1);
        temp_dir[sizeof(temp_dir) - 1] = '\0';

        // Change vers le répertoire précédent
        if (chdir(prev_dir) != 0) {
            perror("cd");
            return 1; 
        }

        // Met à jour prev_dir
        strncpy(prev_dir, temp_dir, sizeof(prev_dir) - 1);
        prev_dir[sizeof(prev_dir) - 1] = '\0';
    }
    // "cd arg" -> Change vers args[1]
    else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return 1; 
        }
    }

    // Met à jour prev_dir après un changement réussi
    strncpy(prev_dir, current_dir, sizeof(prev_dir) - 1);
    prev_dir[sizeof(prev_dir) - 1] = '\0';

    return 0; 
}

