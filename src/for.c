#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "../include/for.h"
#include "../include/builtins.h"


/**
 * Exécute une boucle for sur le contenu d'un répertoire spécifié.
 * 
 * La fonction attend des arguments similaires à une boucle for d'un shell avec la syntaxe :
 * for variable in répertoire { commande }
 * La commande peut référencer l'élément du répertoire avec $F qui sera remplacé par le nom de chaque fichier/dossier.
 *
 * @param args Un tableau de chaînes de caractères contenant les mots de la commande.
 * @param val Une valeur de contrôle utilisée lors de l'appel de la fonction execute_builtin, peut être utilisée pour le contrôle du flux ou le débogage.
 * @return 0 en cas de succès, 1 en cas d'échec avec un message d'erreur.
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
    char *command[256];
    int cmd_index = 0;

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

        cmd_index = 0;
        for (int i = cmd_start; i < cmd_end; i++) {
            char *extend_arg = malloc(strlen(args[i]) * 2 + strlen(filepath) * 5);  
            char *temp = extend_arg;
            char *part = args[i];

            while (*part) {
                char *next_part = strstr(part, "$F");
                if (next_part) {
                    memcpy(temp, part, next_part - part);
                    temp += next_part - part;
                    strcpy(temp, filepath);
                    temp += strlen(filepath);
                    part = next_part + 2; // skip "$F"
                } else {
                    strcpy(temp, part);
                    break;
                }
            }

            command[cmd_index++] = extend_arg;
        }
        command[cmd_index] = NULL;

        int result = execute_builtin(command, val);
        if (result != 0) {
            perror("Erreur : Commande échouée");
        }
        for (int i = 0; i < cmd_index; i++) {
            free(command[i]);
        }
    }

    closedir(dir);
    return 0;
}
