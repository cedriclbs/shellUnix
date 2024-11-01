#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/builtins.h"  // Utilisation du chemin correct vers `builtins.h`

int main() {
    char *line = NULL;         // Stocke la ligne de commande saisie par l'utilisateur
    char *args[10];            // Stocke les arguments de la commande
    size_t len = 0;

    while (1) {
        printf("fsh$ ");       // Affiche le prompt
        getline(&line, &len, stdin);  // Lit la commande saisie par l'utilisateur
        line[strcspn(line, "\n")] = '\0';  // Supprime le caractère de fin de ligne

        // Découpe la ligne en tokens pour obtenir la commande et ses arguments
        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Termine le tableau des arguments par NULL

        // Exécute la commande `cd` si elle est saisie
        if (strcmp(args[0], "cd") == 0) {
            cmd_cd(args);  // Appelle la fonction cd définie dans builtins.c
        }
        // Permet de quitter le shell avec `exit`
        else if (strcmp(args[0], "exit") == 0) {
            break;  // Sort de la boucle et termine le shell
        }
        else {
            printf("Commande non reconnue\n");
        }
    }

    free(line);  // Libère la mémoire allouée pour la ligne de commande
    return 0;
}
