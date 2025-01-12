#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../include/pipelines.h"
#include "../include/builtins.h"
#include "../include/execute.h"
#include "../include/redirections.h"

/**
 * @brief Vérifie s'il y a un caractère `|` hors accolades dans les arguments.
 *
 * Cette fonction parcourt les arguments pour détecter un caractère `|` situé hors
 * des paires d'accolades `{` et `}`.
 *
 * @param args Tableau de chaînes représentant les arguments de la commande.
 * @return 1 s'il y a un caractère `|` hors accolades, sinon 0.
 */
int isTherePipeOutside(char **args) {
    int in_braces = 0;
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "{") == 0) {
            in_braces++;
        } else if (strcmp(args[i], "}") == 0) {
            in_braces--;
        } else if (in_braces == 0 && strcmp(args[i], "|") == 0) {
            return 1;
        }
        i++;
    }
    return 0;
}

/**
 * @brief Copie un tableau de chaînes en un nouveau tableau.
 *
 * Cette fonction alloue dynamiquement un nouveau tableau de chaînes de caractères
 * et y copie les éléments d'un tableau source.
 *
 * @param cur_cmd Tableau source des arguments.
 * @param cur_cmd_size Taille du tableau source.
 * @return Un tableau alloué dynamiquement contenant une copie des arguments, terminé par `NULL`.
 */
static char **copy_cmd_pipe(char *cur_cmd[], int cur_cmd_size) {
    char **cmd = malloc((cur_cmd_size + 1) * sizeof(char *));
    for (int j = 0; j < cur_cmd_size; j++) {
        cmd[j] = cur_cmd[j];
    }
    cmd[cur_cmd_size] = NULL;
    return cmd;
}

/**
 * @brief Divise les arguments sur le délimiteur `|`, en ignorant ceux dans des accolades.
 *
 * Cette fonction découpe les arguments d'une commande en sous-commandes séparées par `|`,
 * tout en tenant compte des accolades `{}` pour éviter de diviser des blocs internes.
 *
 * @param args Tableau de chaînes représentant les arguments de la commande.
 * @param nb Pointeur où stocker le nombre de sous-commandes extraites.
 * @return Un tableau de sous-commandes (chaque sous-commande est un tableau `char**`).
 */
static char ***split_pipe(char *args[], int *nb) {
    char ***ret_tab = malloc(100 * sizeof(char **));
    int count = 0;
    int i = 0;
    int in_braces = 0;

    char *cur_cmd[100];
    int cur_cmd_size = 0;

    while (args[i] != NULL) {
        if (strcmp(args[i], "{") == 0) {
            in_braces++;
        } else if (strcmp(args[i], "}") == 0) {
            in_braces--;
        }
        if (in_braces == 0 && strcmp(args[i], "|") == 0) {
            if (cur_cmd_size > 0) {
                ret_tab[count] = copy_cmd_pipe(cur_cmd, cur_cmd_size);
                count++;
                cur_cmd_size = 0;
            } else {
                return NULL;
            }
        } else {
            cur_cmd[cur_cmd_size] = args[i];
            cur_cmd_size++;
        }
        i++;
    }
    if (cur_cmd_size > 0) {
        ret_tab[count] = copy_cmd_pipe(cur_cmd, cur_cmd_size);
        count++;
    }
    *nb = count;
    return ret_tab;
}

/**
 * @brief Exécute une commande en pipeline.
 *
 * Cette fonction gère un pipeline de commandes (e.g., `cmd1 | cmd2 | cmd3`),
 * en créant les processus nécessaires et en établissant les connexions
 * via des tubes entre les sorties des commandes et les entrées des suivantes.
 *
 * @param args Tableau de chaînes représentant la commande complète.
 * @param val Dernier code de retour (utile pour certaines commandes internes).
 * @return Le code de retour de la dernière commande du pipeline.
 */
int cmd_pipeline(char **args, int val) {
    int nb_cmd = 0;
    char ***cmds = split_pipe(args, &nb_cmd);
    if (cmds == NULL) {
        perror("erreur de syntaxe");
        return 1;
    }
    pid_t pids[nb_cmd];
    int pipefd[2];
    int previous_read_end = -1;

    for (int i = 0; i < nb_cmd; i++) {
        if (i < nb_cmd - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return 1;
            }
        }
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            if (i > 0) {
                dup2(previous_read_end, STDIN_FILENO);
                close(previous_read_end);
            }
            if (i < nb_cmd - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            int cmd_size = 0;
            while (cmds[i][cmd_size] != NULL) {
                cmd_size++;
            }
            int ret = execute_builtin(cmds[i], cmd_size, val);
            exit(ret);
        } else {
            pids[i] = pid;
            if (i > 0) {
                close(previous_read_end);
            }
            if (i < nb_cmd - 1) {
                close(pipefd[1]);
                previous_read_end = pipefd[0];
            }
        }
    }
    int status;
    int ret_value = 0;
    for (int i = 0; i < nb_cmd; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            ret_value = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            ret_value = 128 + WTERMSIG(status);
        }
    }
    for (int i = 0; i < nb_cmd; i++) {
        free(cmds[i]);
    }
    free(cmds);
    return ret_value;
}