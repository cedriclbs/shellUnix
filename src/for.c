#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "../include/for.h"
#include "../include/builtins.h"

int cmd_for(char **args, int val) {
    char *cmd = NULL;         // La commande à exécuter
    char *directory = NULL;   // Le répertoire "words2"
    char *files[256];         // Tableau pour stocker les fichiers
    int file_count = 0;       // Compteur de fichiers
    int i = 0;                // Index pour parcourir args

    // Vérifier que le format est correct : "for F in words2 { cmd $F }"
    if (args[0] == NULL || strcmp(args[0], "for") != 0 || 
        args[1] == NULL || strcmp(args[2], "in") != 0 || 
        args[3] == NULL) {
        fprintf(stderr, "Erreur : Syntaxe incorrecte pour la boucle 'for'.\n");
        return 1;
    }

    directory = args[3]; // Récupérer le répertoire spécifié après "in"

    // Ouvrir le répertoire
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du répertoire");
        return 1;
    }

    // Lire les fichiers dans le répertoire
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignorer les fichiers spéciaux "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Ajouter le fichier à la liste
        files[file_count++] = strdup(entry->d_name);
        if (file_count >= 256) {
            fprintf(stderr, "Erreur : Trop de fichiers dans le répertoire '%s'.\n", directory);
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);

    if (file_count == 0) {
        fprintf(stderr, "Erreur : Aucun fichier trouvé dans le répertoire '%s'.\n", directory);
        return 1;
    }

    // Rechercher la commande entre "{" et "}"
    int cmd_start = -1, cmd_end = -1;
    for (i = 4; args[i] != NULL; i++) {
        if (strcmp(args[i], "{") == 0) {
            cmd_start = i + 1; // Début de la commande après "{"
        } else if (strcmp(args[i], "}") == 0) {
            cmd_end = i; // Fin de la commande avant "}"
            break;
        }
    }

    if (cmd_start == -1 || cmd_end == -1 || cmd_start >= cmd_end) {
        fprintf(stderr, "Erreur : Délimiteurs '{' et '}' mal placés ou absents.\n");
        return 1;
    }

    // Vérifier la syntaxe de la commande
    cmd = args[cmd_start]; // Récupérer la commande (exemple : "ftype")
    if (cmd_start + 1 >= cmd_end || strcmp(args[cmd_start + 1], "$F") != 0) {
        fprintf(stderr, "Erreur : Syntaxe incorrecte, '$F' attendu après la commande.\n");
        return 1;
    }

    // Préparer l'exécution de la commande pour chaque fichier
    char *arg1[3];
    arg1[0] = cmd;
    arg1[2] = NULL;

    for (int j = 0; j < file_count; j++) {
        // Construire le chemin complet du fichier si nécessaire
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", directory, files[j]);
        arg1[1] = path;

        // Exécuter la commande
        int result = execute_builtin(arg1, val);
        if (result != 0) {
            fprintf(stderr, "Erreur : Commande '%s %s' échouée avec statut %d\n", arg1[0], arg1[1], result);
        }

        // Libérer la mémoire pour le fichier
        free(files[j]);
    }

    return 0;
}
