#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "../include/for.h"
#include "../include/builtins.h"

/**
 * @brief Implémente une boucle `for` pour exécuter une commande sur chaque fichier dans un répertoire.
 *
 * Cette fonction interprète une syntaxe de type :
 * `for F in <directory> { <command> $F }`
 * Elle parcourt tous les fichiers dans le répertoire `<directory>` et exécute la commande spécifiée
 * pour chaque fichier, en remplaçant `$F` par le nom complet du fichier.
 *
 * @param args Tableau d'arguments de la commande. Exemple :
 *             `args = {"for", "F", "in", "test_dir", "{", "echo", "$F", "}"}`
 * @param val  Dernière valeur de retour connue, utilisée par d'autres commandes internes.
 * @return Un entier représentant le statut de l'exécution :
 *         - 0 si toutes les commandes s'exécutent avec succès.
 *         - 1 en cas d'erreur (ex. syntaxe incorrecte, répertoire inaccessible, etc.).
 */
int cmd_for(char **args, int val) {
    char *directory = NULL;   // Le répertoire "words2"
    char *files[256];         // Tableau pour stocker les fichiers
    int file_count = 0;       // Compteur de fichiers
    int i = 0;                // Index pour parcourir args

    // Vérifie que le format est correct : "for F in words2 { cmd $F }"
    if (args[0] == NULL || strcmp(args[0], "for") != 0 || 
        args[1] == NULL || strcmp(args[2], "in") != 0 || 
        args[3] == NULL) {
        perror("Erreur : Syntaxe incorrecte pour la boucle 'for'");
        return 1;
    }

    directory = args[3]; // Récupère le répertoire spécifié après "in"

    // Ouvre le répertoire
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du répertoire");
        return 1;
    }

    // Lit les fichiers dans le répertoire
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignore les fichiers spéciaux "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Ajoute le fichier à la liste
        files[file_count++] = strdup(entry->d_name);
        if (file_count >= 256) {
            perror("Erreur : Trop de fichiers dans le répertoire");
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);

    if (file_count == 0) {
        perror("Erreur : Aucun fichier trouvé dans le répertoire");
        return 1;
    }

    // Recherche la commande entre "{" et "}"
    int cmd_start = -1, cmd_end = -1;
    for (i = 4; args[i] != NULL; i++) {
        if (strcmp(args[i], "{") == 0) {
            cmd_start = i + 1; 
        } else if (strcmp(args[i], "}") == 0) {
            cmd_end = i;
            break;
        }
    }

    if (cmd_start == -1 || cmd_end == -1 || cmd_start >= cmd_end) {
        perror("Erreur : Délimiteurs '{' et '}' mal placés ou absents");
        return 1;
    }

    // Vérifie la syntaxe de la commande
    int valF = 0;
    for (int j = cmd_start ; j < cmd_end; j++){
        if (strcmp(args[j], "$F") == 0){
            valF = 1;
            break;
        }
    }
    if (!valF){
        perror("Erreur : Syntaxe incorrecte, '$F' attendu dans la commande");
        return 1;
    }

    // Prépare l'exécution de la commande pour chaque fichier
    for (int k = 0; k < file_count; k++) {
        // Construit le chemin complet du fichier si nécessaire
        char *arg1[cmd_end - cmd_start + 2];
        int index = 0;

        for (int t = cmd_start ; t < cmd_end ; t++){
            if (strcmp(args[t], "$F") == 0){
                char *path = malloc(512);
                snprintf(path, 512, "%s/%s", directory, files[k]);
                arg1[index++] = path;
            } else {
                arg1[index++] = strdup(args[t]);
            }
        }
        arg1[index] = NULL; 


        
        // Exécute la commande
        int result = execute_builtin(arg1, val);
        if (result != 0) {
            perror("Erreur : Commande échouée");
        }

        // Libère la mémoire pour le fichier
        for (int t = 0; t < index; t++) {
            free(arg1[t]);
        }
    }

    for (int j = 0; j < file_count; j++) {
        free(files[j]);
    }


    return 0;
}
