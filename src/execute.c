#include "../include/execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/**
 * @brief Exécute une commande en gérant les redirections d'entrée, de sortie et d'erreur.
 * 
 * Cette fonction gère les redirections suivantes :
 * - `<` pour rediriger l'entrée depuis un fichier.
 * - `>` pour rediriger la sortie vers un fichier en mode écrasement.
 * - `>>` pour rediriger la sortie vers un fichier en mode ajout.
 * - `2>` pour rediriger les erreurs standard vers un fichier en mode écrasement.
 * - `2>>` pour rediriger les erreurs standard vers un fichier en mode ajout.
 * - `&>` pour rediriger la sortie et les erreurs standard vers un fichier en mode écrasement.
 * - `&>>` pour rediriger la sortie et les erreurs standard vers un fichier en mode ajout.
 * 
 * @param args Un tableau de chaînes de caractères représentant la commande et ses arguments.
 *             Les redirections doivent être spécifiées sous forme d'arguments dans le tableau.
 * @return int Le code de retour du processus exécuté.
 *             - Retourne le code de sortie du processus enfant si celui-ci s'est terminé normalement.
 *             - Retourne 128 + numéro du signal si le processus enfant a été terminé par un signal.
 *             - Retourne 1 en cas d'erreur de fork ou d'exécution.
 */
int execute_command_with_redirection(char **args) {
    char *input_file = NULL;       /**< Fichier pour la redirection d'entrée */
    char *output_file = NULL;      /**< Fichier pour la redirection de sortie */
    char *error_file = NULL;       /**< Fichier pour la redirection des erreurs */
    int output_append = 0;         /**< Indicateur d'ajout à la sortie */
    int error_append = 0;          /**< Indicateur d'ajout pour les erreurs */
    int combined_output = 0;       /**< Indique si sortie et erreurs sont combinées */

    // Analyse des arguments pour détecter les redirections
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            i++;
            input_file = args[i];
        } else if (strcmp(args[i], ">") == 0) {
            i++;
            output_file = args[i];
            output_append = 0;
        } else if (strcmp(args[i], ">>") == 0) {
            i++;
            output_file = args[i];
            output_append = 1;
        } else if (strcmp(args[i], "2>") == 0) {
            i++;
            error_file = args[i];
            error_append = 0;
        } else if (strcmp(args[i], "2>>") == 0) {
            i++;
            error_file = args[i];
            error_append = 1;
        } else if (strcmp(args[i], "&>") == 0) {
            i++;
            output_file = args[i];
            error_file = args[i];
            combined_output = 1;
            output_append = 0;
        } else if (strcmp(args[i], "&>>") == 0) {
            i++;
            output_file = args[i];
            error_file = args[i];
            combined_output = 1;
            output_append = 1;
        }
        i++;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Redirection d'entrée
        if (input_file != NULL) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("Erreur d'ouverture du fichier d'entrée");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // Redirection de sortie
        if (output_file != NULL) {
            int fd_out;
            if (output_append) {
                fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (fd_out == -1) {
                perror("Erreur d'ouverture du fichier de sortie");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            if (combined_output) {
                dup2(fd_out, STDERR_FILENO);
            }
            close(fd_out);
        }

        // Redirection des erreurs
        if (error_file != NULL && !combined_output) {
            int fd_err;
            if (error_append) {
                fd_err = open(error_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                fd_err = open(error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (fd_err == -1) {
                perror("Erreur d'ouverture du fichier d'erreur");
                exit(1);
            }
            dup2(fd_err, STDERR_FILENO);
            close(fd_err);
        }

        // Exécution de la commande
        execvp(args[0], args);
        perror("Erreur d'exécution de la commande");
        exit(1);
    } else if (pid > 0) {
        // Processus parent : attend la fin du processus enfant
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            return 128 + WTERMSIG(status);
        }
    } else {
        perror("Erreur de fork");
        return 1;
    }

    return 0;
}
