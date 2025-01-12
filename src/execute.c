#include "../include/execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../include/redirections.h"
#include "../include/signals.h"

/**
 * @file execute_command_with_redirection.c
 * @brief Exécute une commande avec gestion des redirections en utilisant `fork` et `execvp`.
 *
 * Ce fichier définit une fonction permettant d'exécuter une commande dans un processus enfant
 * tout en gérant les redirections standard et les signaux.
 */

/**
 * @brief Exécute une commande avec des redirections éventuelles en utilisant `fork` et `execvp`.
 *
 * Cette fonction crée un processus enfant pour exécuter une commande donnée.
 * Les redirections doivent être déjà configurées avant l'appel à cette fonction.
 * En cas d'échec de l'exécution, un message d'erreur est affiché avec `perror`,
 * et le processus enfant se termine avec un code d'erreur.
 *
 * Le processus parent attend la fin de l'exécution du processus enfant :
 * - Si le processus enfant est interrompu par un signal, cette fonction retourne 128 + numéro du signal.
 * - Si le processus enfant se termine normalement, le code de retour du processus enfant est renvoyé.
 * - En cas d'échec du `fork`, la fonction retourne 1.
 *
 * @param args Tableau de chaînes représentant la commande à exécuter et ses arguments.
 *             Le tableau doit être terminé par un pointeur `NULL`.
 *
 * @return Un entier représentant le code de retour :
 *         - Code de retour du processus enfant si `execvp` réussit.
 *         - `128 + numéro du signal` si le processus enfant est interrompu par un signal.
 *         - 1 en cas d'échec de `fork` ou de `execvp`.
 */
int execute_command_with_redirection(char **args) {
    pid_t pid = fork(); 

    if (pid == 0) { 
        unblockSignals();

        // Exécution de la commande avec `execvp`
        execvp(args[0], args);

        // Si `execvp` échoue, afficher l'erreur et quitter avec un code d'erreur
        perror("redirect_exec");
        exit(1);
    } else if (pid > 0) { // Processus parent
        int status;
        waitpid(pid, &status, 0);

        // Vérifie si le processus enfant a été interrompu par un signal
        if (WIFSIGNALED(status)) {
            updatesig(WTERMSIG(status)); 
            return 128 + WTERMSIG(status); 
        }

        // Vérifie si le processus enfant s'est terminé normalement
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status); 
        }
    } else { 
        perror("Erreur de fork");
        return 1;
    }

    return 0; 
