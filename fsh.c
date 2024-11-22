#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>  // Inclure pour add_history
#include "prompt.h"
#include "exit.h"

int main() {
    char *ligne;
    int val = 0;

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
        
        // Vérification de la commande "exit"
        if (strncmp(ligne, "exit", 4) == 0) {
            int nb = test_exit(ligne);
            free(ligne);
            exit(nb);
        }

        // Libérer la mémoire
        free(ligne);
        
        // Réaffiche le prompt 
        rl_redisplay();
    }

    return 0;
}
