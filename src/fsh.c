#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "prompt.h"
#include "exit.h"
#include "builtins.h"

#define MAX_ARGS 100

int main() {
    char *ligne;
    int val = 0;  // Code de retour du shell, initialisé à 0 (succès)
    char *args[MAX_ARGS];
    int i;

    rl_outstream = stderr;  // Rediriger la sortie de readline vers stderr

    // Boucle principale du shell
    while (1) {
        // Affichage du prompt
        char *prompt = getPrompt(val);

        // Lecture de l'entrée
        ligne = readline(prompt);  // Utilise readline pour lire l'entrée
        free(prompt);
        // Vérification de NULL pour éviter une erreur de segmentation
        if (ligne == NULL) {
            printf("\n");  // Gérer Ctrl + D
            break;  // Sortir de la boucle si l'entrée est NULL
        }

        // Ajoute l'entrée à l'historique
        add_history(ligne);

        // Découper la ligne en arguments
        i = 0;
        char *token = strtok(ligne, " ()");  // délimiteurs
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");  // Continuer à découper la ligne
        }
        args[i] = NULL;  // Terminer le tableau d'arguments par NULL

        // Exécuter la commande interne et récupérer le code de retour
        val = execute_builtin(args, val);  // Exécute la commande et met à jour 'val'

        // Libérer la mémoire
        free(ligne);

        // Réafficher le prompt
        rl_redisplay();
    }

    return val;  // Retourne le code de retour du shell
}
