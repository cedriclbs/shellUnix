#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>  // Inclure pour add_history
#include "prompt.h"
#include "exit.h"
#include "builtins.h"

#define MAX_ARGS 100

int main() {
    char *ligne;
    int val = 0;
    char *args[MAX_ARGS];
    int i;

    rl_outstream = stderr; // Redirige la sortie de readline vers stderr

    // Boucle principale du shell
    while (1) {
        // Affichage du prompt
        char *prompt = getPrompt(val);

        // Lecture de l'entrée
        ligne = readline(prompt);  // Utilise readline pour lire l'entrée

        // Vérification de NULL pour éviter une erreur de segmentation
        if (ligne == NULL) {
            printf("\n"); // Gérer Ctrl + D
            break; // Sortir de la boucle si l'entrée est NULL
        }

        // Ajoute l'entrée à l'historique
        add_history(ligne);

        // Découper la ligne en arguments
        i = 0;
        char *token = strtok(ligne, " _()");  // délimiteurs
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");  // Continuer à découper la ligne
        }
        args[i] = NULL;  // Terminer le tableau d'arguments par NULL
        
        int nb = execute_builtin(args);
    
        // Libérer la mémoire
        free(ligne);
        
        // Réaffiche le prompt 
        rl_redisplay();

        if(nb == 1){
            return 1;
        }
    }

    return 0;
}
