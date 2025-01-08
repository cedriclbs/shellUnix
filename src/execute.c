#include "../include/execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../include/redirections.h"
#include"../include/signals.h"

/**
 * Exécute une commande avec des redirections éventuelles en utilisant les appels système fork et execvp.
 * 
 * Cette fonction crée un processus enfant qui exécute la commande spécifiée
 * en utilisant execvp. Les erreurs d'exécution sont capturées et signalées via perror.
 * Le processus parent attend la fin de l'exécution du processus enfant et retourne
 * le code de sortie de ce dernier. Si le processus enfant termine à cause d'un signal,
 * cette fonction retourne 128 plus le numéro du signal.
 *
 * @param args Tableau de chaînes contenant la commande à exécuter et ses arguments.
 * @return Le code de sortie du processus enfant, ou 1 en cas d'échec du fork.
 */

int execute_command_with_redirection(char **args) {
    pid_t pid = fork();

    if (pid == 0) { // Processus enfant
        unblockSigterm();
        // Exécution de la commande
        execvp(args[0], args);
        perror("redirect_exec");
        exit(1);
    } else if (pid > 0) { // Processus parent
        int status;
        waitpid(pid, &status, 0); // Attente de la fin du processus enfant
        if (WIFSIGNALED(status)) {
            updatesig(WTERMSIG(status));
            return 128 + WTERMSIG(status);
        }else if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    } else {
        perror("Erreur de fork");
        return 1;
    }
    return 0;
}