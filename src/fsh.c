#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "prompt.h"
#include "exit.h"
#include "builtins.h"

#define MAX_ARGS 100

/**
 * @brief Point d'entrée principal du programme pour un shell interactif.
 *
 * Ce programme implémente un shell simple capable de :
 * - Lire des commandes depuis l'entrée utilisateur.
 * - Analyser et exécuter des commandes internes définies dans le programme.
 * - Gérer l'historique des commandes avec la bibliothèque readline.
 * 
 * Fonctionnalités principales :
 * - Affiche un prompt personnalisé (généré par `getPrompt`).
 * - Découpe les commandes saisies en arguments pour les exécuter.
 * - Exécute les commandes internes via `execute_builtin`.
 * - Supporte les signaux de fin d'entrée (`Ctrl+D`) en terminant proprement.
 *
 * @return Le code de retour du shell (dernier code de retour d'une commande interne).
 */
int main() {
    char *ligne;
    int val = 0;  // Code de retour du shell, initialisé à 0 (succès)
    char *args[MAX_ARGS];
    int argc;

    rl_outstream = stderr;  // Redirige la sortie de readline vers stderr

    // Boucle principale du shell
    while (1) {
        // Affichage du prompt
        char *prompt = getPrompt(val);

        // Lecture de l'entrée
        ligne = readline(prompt); 
        free(prompt);

        // Vérification de NULL pour éviter une erreur de segmentation
        if (ligne == NULL) {
            printf("\n");  // Gère Ctrl + D
            break;  
        }

        // Ajoute l'entrée à l'historique
        add_history(ligne);

        // Découpe la ligne en arguments
        argc=0;
        char *token = strtok(ligne, " ()");  // Délimiteurs
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " ");  
        }
        args[argc] = NULL;  

        // Exécute la commande interne et récupère le code de retour
        val = execute_builtin(args, argc,val);  

        // Libère la mémoire
        free(ligne);

        // Réaffiche le prompt
        rl_redisplay();
    }

    return val;
}
