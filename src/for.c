#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "../include/for.h"
#include "../include/builtins.h"

/**
 * Implémente une boucle `for` pour exécuter une commande sur chaque fichier dans un répertoire.
 * 
 * Cette fonction interprète une syntaxe de type :
 * `for F in <directory> { <command> $F }`
 * Elle parcourt tous les fichiers dans le répertoire `<directory>` et exécute la commande spécifiée
 * pour chaque fichier, en remplaçant `$F` par le chemin complet du fichier.
 */
int cmd_for(char **args, int val) {
    if (args[0] == NULL || strcmp(args[0], "for") != 0 || 
        args[1] == NULL || strcmp(args[2], "in") != 0 || 
        args[3] == NULL || args[4] == NULL || strcmp(args[4], "{") != 0) {
        perror("Erreur : Syntaxe incorrecte pour la boucle 'for'");
        return 1;
    }

    char *directory = args[3];
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du répertoire");
        return 1;
    }

    struct dirent *entry;
    char *full_command[256];
    int cmd_index = 0;

    // Trouve le début et la fin de la commande dans les arguments
    int cmd_start = -1, cmd_end = -1;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "{") == 0) {
            cmd_start = i + 1;
        } else if (strcmp(args[i], "}") == 0) {
            cmd_end = i;
            break;
        }
    }

    if (cmd_start == -1 || cmd_end == -1 || cmd_start >= cmd_end) {
        perror("Erreur : Délimiteurs '{' et '}' mal placés ou absents");
        closedir(dir);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.') {
        continue;
    }

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

    // Remplace $F dans la commande et construit la nouvelle commande
    cmd_index = 0;
    for (int i = cmd_start; i < cmd_end; i++) {
        if (strcmp(args[i], "$F") == 0) {
            full_command[cmd_index++] = filepath;
        } else {
            full_command[cmd_index++] = args[i];
        }
    }
    full_command[cmd_index] = NULL;  // Terminaison de la commande

    // Exécute la commande
    int result = execute_builtin(full_command, val);
        if (result != 0) {
            perror("Erreur : Commande échouée");
        }
    }

    closedir(dir);
    return 0;
}
