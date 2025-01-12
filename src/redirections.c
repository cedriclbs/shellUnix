#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/redirections.h"

/**
 * Analyse les redirections dans les arguments d'une commande et extrait les arguments sans les redirections.
 * Cette fonction parcourt le tableau d'arguments pour identifier les opérateurs de redirection comme '<', '>', etc.,
 * et extrait les chemins de fichiers associés à ces opérateurs. Les arguments non liés à la redirection sont stockés
 * dans un nouveau tableau qui est ensuite retourné. Les fichiers spécifiés pour les redirections sont ouverts avec
 * les flags appropriés.
 *
 * @param args Tableau de chaînes représentant la commande et ses arguments.
 * @param input_file Pointeur vers une chaîne où stocker le chemin du fichier d'entrée si spécifié.
 * @param output_file Pointeur vers une chaîne où stocker le chemin du fichier de sortie si spécifié.
 * @param error_file Pointeur vers une chaîne où stocker le chemin du fichier d'erreur si spécifié.
 * @param output_flags Pointeur vers un entier pour stocker les flags de la redirection de sortie.
 * @param error_flags Pointeur vers un entier pour stocker les flags de la redirection des erreurs.
 * @param cmd_size Pointeur vers une variable de type size_t pour stocker la taille du tableau retourné.
 * @return Tableau de chaînes contenant les arguments de la commande sans les redirections. NULL en cas d'erreur.
 */
char **parse_redirections(char **args, char **input_file, char **output_file, char **error_file,int *output_flags, int *error_flags, size_t *cmd_size) {
    *output_flags = O_WRONLY | O_CREAT; 
    *error_flags = O_WRONLY | O_CREAT;  
    size_t cmd_s = 0;                   

    // Calcule la taille de cmd_args (avant les symboles de redirection)
    while (args[cmd_s] != NULL) {
        if (strcmp(args[cmd_s], "<") == 0 ||
            strcmp(args[cmd_s], ">") == 0 ||
            strcmp(args[cmd_s], ">|") == 0 ||
            strcmp(args[cmd_s], ">>") == 0 ||
            strcmp(args[cmd_s], "2>") == 0 ||
            strcmp(args[cmd_s], "2>|") == 0 ||
            strcmp(args[cmd_s], "2>>") == 0) {
            break;
        }
        (cmd_s)++;
    }

    // Alloue de la mémoire pour cmd_args (taille + 1 pour NULL terminal)
    char **cmd_args = malloc((cmd_s + 1) * sizeof(char *));
    if (cmd_args == NULL) {
        perror("Erreur d'allocation mémoire pour cmd_args");
        return NULL;
    }

    // Copie les arguments avant les redirections dans cmd_args
    for (size_t i = 0; i < cmd_s; i++) {
        cmd_args[i] = args[i];
    }
    cmd_args[cmd_s] = NULL; 

    // Parcour les arguments restants pour détecter les redirections
    for (size_t i = cmd_s; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection d'entrée");
                free(cmd_args);  
                return NULL;
            }
            *input_file = args[i + 1];
            i++;
        } else if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection de sortie");
                free(cmd_args);  
                return NULL;
            }
            *output_file = args[i + 1];
            *output_flags |= O_EXCL; 
            i++;
        } else if (strcmp(args[i], ">|") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection de sortie avec écrasement");
                free(cmd_args); 
                return NULL;
            }
            *output_file = args[i + 1];
            *output_flags |= O_TRUNC; 
            i++;
        } else if (strcmp(args[i], ">>") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection de sortie en concaténation");
                free(cmd_args); 
                return NULL;
            }
            *output_file = args[i + 1];
            *output_flags |= O_APPEND; 
            i++;
        } else if (strcmp(args[i], "2>") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection des erreurs");
                free(cmd_args);  
                return NULL;
            }
            *error_file = args[i + 1];
            *error_flags |= O_EXCL; 
            i++;
        } else if (strcmp(args[i], "2>|") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection des erreurs avec écrasement");
                free(cmd_args);  
                return NULL;
            }
            *error_file = args[i + 1];
            *error_flags |= O_TRUNC; 
            i++;
        } else if (strcmp(args[i], "2>>") == 0) {
            if (args[i + 1] == NULL) {
                perror("Erreur : fichier manquant pour la redirection des erreurs en concaténation");
                free(cmd_args);  
                return NULL;
            }
            *error_file = args[i + 1];
            *error_flags |= O_APPEND; 
            i++;
        }
    }
    if (cmd_s != 0) {
        *cmd_size = cmd_s;
    }
    else{
        free(cmd_args);
        return NULL;
    }
    return cmd_args;
}

/**
 * Configure les redirections de fichiers pour l'exécution d'une commande basée sur les chemins et les flags fournis.
 * Les fichiers spécifiés sont ouverts avec les flags appropriés, et les descripteurs de fichiers standards (entrée,
 * sortie, et erreur standard) sont redirigés vers ces fichiers. Cette fonction assure que toutes les ouvertures et
 * redirections sont correctement gérées et les fichiers sont fermés après leur duplication pour éviter les fuites
 * de descripteurs de fichiers.
 *
 * @param input_file Chemin vers le fichier d'entrée, ou NULL si aucune redirection d'entrée n'est souhaitée.
 * @param output_file Chemin vers le fichier de sortie, ou NULL si aucune redirection de sortie n'est souhaitée.
 * @param error_file Chemin vers le fichier d'erreurs, ou NULL si aucune redirection d'erreurs n'est souhaitée.
 * @param output_flags Flags de fichier pour ouvrir le fichier de sortie.
 * @param error_flags Flags de fichier pour ouvrir le fichier d'erreurs.
 * @return 0 en cas de succès, 1 en cas d'erreur avec les fichiers ou les descripteurs de fichier.
 */
int setup_redirections(const char *input_file, const char *output_file, const char *error_file,int output_flags, int error_flags) {
    if (input_file) {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in == -1) {
            perror("Erreur à l'ouverture du fichier d'entrée");
            return 1;
        }
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("Erreur dans la redirection d'entrée");
            close(fd_in);
            return 1;
        }
        close(fd_in);
    }

    if (output_file) {
        int fd_out = open(output_file, output_flags, 0644);
        if (fd_out == -1) {
            perror("Erreur à l'ouverture du fichier de sortie");
            return 1;
        }
        if (dup2(fd_out, STDOUT_FILENO) == -1) {
            perror("Erreur dans la redirection de sortie");
            close(fd_out);
            return 1;
        }
        close(fd_out);
    }
    if (error_file) {
        int fd_err = open(error_file, error_flags, 0644);
        if (fd_err == -1) {
            perror("Erreur à l'ouverture du fichier des erreurs");
            return 1;
        }
        if (dup2(fd_err, STDERR_FILENO) == -1) {
            perror("Erreur dans la redirection des erreurs");
            close(fd_err);
            return 1;
        }
        close(fd_err);
    }
    return 0;
}