#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/cd.h"

/**
 * @brief Implémente la commande interne `cd` pour changer de répertoire.
 *
 * Cette fonction gère plusieurs cas de la commande `cd` :
 * - `cd` sans argument : bascule vers le répertoire défini par la variable d'environnement `HOME`.
 * - `cd -` : bascule vers le dernier répertoire visité.
 * - `cd <path>` : bascule vers le répertoire spécifié dans `path`.
 *
 * En cas d'erreur (par exemple, répertoire introuvable ou problème d'accès),
 * un message d'erreur est affiché sur la sortie d'erreur standard (`stderr`),
 * et la fonction retourne un code d'erreur.
 *
 * @param args Tableau contenant les arguments de la commande. 
 *             - `args[0]` : la commande elle-même (`cd`).
 *             - `args[1]` : (optionnel) le chemin ou l'option (`-`).
 *
 * @return Un entier représentant le code de retour :
 *         - 0 si l'opération réussit.
 *         - 1 en cas d'erreur (par exemple, répertoire introuvable ou accès refusé).
 */
int cmd_cd(char **args) {
    static char prev_dir[1024] = ""; 
    char current_dir[1024];        
    char temp_dir[1024];           
    char *path_h;

    // Sauvegarde le répertoire actuel avant de changer
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("cd");
        return 1;
    }

    // "cd" sans argument -> aller au répertoire HOME
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
    // "cd -" -> aller au répertoire précédent
    else if (strcmp(args[1], "-") == 0) {
        if (strlen(prev_dir) == 0) {
            fprintf(stderr, "cd: Aucun répertoire précédent\n");
            return 1;
        }

        // Sauvegarde temporairement le répertoire actuel
        strncpy(temp_dir, current_dir, sizeof(temp_dir) - 1);
        temp_dir[sizeof(temp_dir) - 1] = '\0';

        // Change vers le répertoire précédent
        if (chdir(prev_dir) != 0) {
            perror("cd");
            return 1;
        }

        // Met à jour `prev_dir`
        strncpy(prev_dir, temp_dir, sizeof(prev_dir) - 1);
        prev_dir[sizeof(prev_dir) - 1] = '\0';
    }
    // "cd <arg>" -> changer vers `args[1]`
    else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return 1;
        }
    }

    // Met à jour `prev_dir` après un changement réussi
    strncpy(prev_dir, current_dir, sizeof(prev_dir) - 1);
    prev_dir[sizeof(prev_dir) - 1] = '\0';

    return 0;
}
