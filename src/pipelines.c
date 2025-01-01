#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "builtins.h"
#include "../include/cmd_line.h"

#define MAX_LENGTH 1024

char **add_arg(char **cmds, char *arg) {
    int size = 0;
    while (cmds[size] != NULL) {
        size++;
    }

    char **new_cmds = malloc((size + 2) * sizeof(char *)); 
    if (new_cmds == NULL) {
        perror("malloc");
        return NULL; 
    }

    for (int i = 0; i < size; i++) {
        new_cmds[i] = cmds[i];
    }

    new_cmds[size] = arg;
    new_cmds[size + 1] = NULL;

    return new_cmds; 
}

int execute_pipelines(char ***cmds, int argc) {
    int pipes[argc - 1][2]; // Créer des pipes pour chaque commande sauf la dernière
    pid_t pids[argc]; // Pour stocker les PID des processus enfants
    int ret = 0;

    // Initialisation des pipes
    for (int i = 0; i < argc - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipelines : Erreur pipe");
            return 1;
        }
    }

    for (int i = 0; i < argc; i++) {
        pids[i] = fork();
        switch (pids[i]) {
            case -1:
                perror("Erreur fork");
                exit(1);

            case 0:  // Code du processus enfant
                // Redirection de la sortie standard
                if (i < argc - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                        perror("pipelines: Erreur dup2 pour STDOUT");
                        exit(1);
                    }
                }

                // Redirection des erreurs standard
                if (i < argc - 1) {  // Si ce n'est pas la dernière commande
                    if (dup2(pipes[i][1], STDERR_FILENO) == -1) {
                        perror("pipelines: Erreur dup2 pour STDERR");
                        exit(1);
                    }
                }

                // Redirection de l'entrée standard
                if (i > 0) {
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                        perror("pipelines: Erreur dup2 pour STDIN");
                        exit(1);
                    }
                }

                // Fermer tous les descripteurs de pipe
                for (int j = 0; j < argc - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Exécuter la commande avec execute_builtin
                
                int r =0;
                
                    int cmd_size = 0;
                    // Calcule la taille de la commande actuelle (le nombre d'arguments)
                    while (cmds[i][cmd_size] != NULL) {
                        cmd_size++;
                    }
                    
                    r = execute_builtin(cmds[i], cmd_size, r);  
                
                exit(r);  // Terminer le processus enfant avec le code de retour

            default:  // Code du processus parent
                // Fermer les descripteurs de pipe inutilisés
                if (i < argc - 1) {
                    close(pipes[i][1]); // Fermer le descripteur d'écriture
                }
                if (i > 0) {
                    close(pipes[i - 1][0]); // Fermer le descripteur de lecture du pipe précédent
                }
                break;
        }
    }

    // Attendre que tous les processus enfants se terminent
    for (int i = 0; i < argc; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            ret = WEXITSTATUS(status);
        }
    }

    return ret;
}

int cmd_pipelines(char **args) {
    int argc = 0;
    char ***cmds = split_cmd(args, &argc, "|");  

    if (cmds == NULL) {
        return 1;
    }

    /* for (int i = 0; i < argc; i++) {
        printf("commande : ");
        for(int j=0; cmds[i][j]!=NULL;j++){
            printf("%s ",cmds[i][j]);  
        }
        printf("\n");
    } */

    int ret = execute_pipelines(cmds, argc);

    for (int i = 0; i < argc; i++) {
        free(cmds[i]);  
    }
    free(cmds);  

    return ret;
}
