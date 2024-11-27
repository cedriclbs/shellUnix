#include "execute.h"

void execute_command_with_redirection(char *command) {
    char *args[100];
    char *input_file = NULL;
    char *output_file = NULL;
    char *error_file = NULL;
    int output_append = 0;
    int error_append = 0;
    int combined_output = 0;

    // sépare la commande et les arguments
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
        // vérifie les symboles de redirection
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) input_file = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) output_file = token;
            output_append = 0;
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) output_file = token;
            output_append = 1;
        } else if (strcmp(token, "2>") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) error_file = token;
            error_append = 0;
        } else if (strcmp(token, "2>>") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) error_file = token;
            error_append = 1;
        } else if (strcmp(token, "&>") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                output_file = token;
                error_file = token;
                combined_output = 1;
                output_append = 0;
            }
        } else if (strcmp(token, "&>>") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                output_file = token;
                error_file = token;
                combined_output = 1;
                output_append = 1;
            }
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        // le processus enfant redirige l'entrée et la sortie si nécessaire
        if (input_file != NULL) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("Erreur d'ouverture du fichier d'entrée");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
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

        // exécute la commande
        execvp(args[0], args);
        perror("Erreur d'exécution de la commande");
        exit(1);
    } else if (pid > 0) {
        // le parent attend la fin du processus enfant
        wait(NULL);
    } else {
        perror("Erreur de fork");
    }
}
