#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "builtins.h"
#include "signal.h"
#include "signals.h"

/**
 * @file cmd_line.c
 * @brief Gestion et exécution de commandes shell, avec prise en charge des séparateurs (`;`) et des pipelines (`|`).
 */

/**
 * @brief Alloue et copie les éléments d'une commande dans un nouveau tableau de chaînes.
 *
 * Cette fonction est utilisée pour créer un tableau de chaînes de caractères 
 * prêt à être utilisé avec les appels système `exec`.
 *
 * @param cur_cmd Tableau des chaînes de caractères à copier.
 * @param cur_cmd_size Taille du tableau `cur_cmd`.
 * @return Pointeur vers le nouveau tableau de chaînes de caractères, 
 *         terminé par un pointeur `NULL`.
 */
char **copy_cmd(char *cur_cmd[], int cur_cmd_size) {
    char **cmd = malloc((cur_cmd_size + 1) * sizeof(char *));
    for (int j = 0; j < cur_cmd_size; j++) {
        cmd[j] = cur_cmd[j];
    }
    cmd[cur_cmd_size] = NULL; // Ajout du pointeur NULL à la fin.
    return cmd;
}

/**
 * @brief Divise une liste d'arguments en sous-commandes selon un délimiteur.
 *
 * La division respecte les paires d'accolades `{}` pour ignorer les délimiteurs 
 * situés à l'intérieur. Par exemple, une commande `cmd1 { cmd2 ; cmd3 } cmd4 ;` 
 * divise correctement en `cmd1`, `{ cmd2 ; cmd3 }`, et `cmd4`.
 *
 * @param args Tableau des arguments représentant la commande entière.
 * @param nb Pointeur pour stocker le nombre de sous-commandes détectées.
 * @param delimiter Le délimiteur utilisé pour diviser (par exemple, `;` ou `|`).
 * @return Tableau de tableaux de chaînes, chaque sous-tableau représentant une sous-commande.
 *         Retourne `NULL` si une erreur de syntaxe est détectée.
 */
char ***split_cmd(char *args[], int *nb, char *delimiter) {
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

        if (in_braces == 0 && strcmp(args[i], delimiter) == 0) {
            if (cur_cmd_size > 0) {
                ret_tab[count] = copy_cmd(cur_cmd, cur_cmd_size);
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

    if (strcmp(delimiter, "|") == 0 && cur_cmd_size == 0) {
        perror("pipelines: Erreur de syntaxe");
        return NULL;
    }

    if (cur_cmd_size > 0) {
        ret_tab[count] = copy_cmd(cur_cmd, cur_cmd_size);
        count++;
    }

    *nb = count;
    return ret_tab;
}

/**
 * @brief Exécute une commande en créant un processus enfant.
 *
 * Cette fonction utilise `fork` pour créer un processus enfant, 
 * puis exécute la commande dans l'enfant en appelant une commande interne ou externe.
 * Le processus parent attend la fin du processus enfant via `waitpid`.
 *
 * @param cmd Tableau de chaînes représentant la commande à exécuter.
 * @param argc Nombre d'arguments dans la commande.
 * @param val Valeur passée aux commandes internes (par exemple, `for` ou `exit`).
 * @return Code de sortie de la commande exécutée.
 */
int execute_cmd(char **cmd, int argc, int val) {
    pid_t pid = fork(); 
    int ret = 0;

    switch (pid) {
        case -1: 
            perror("Erreur fork");
            exit(1);

        case 0: 
            unblockSignals();
            ret = execute_builtin(cmd, argc, val);
            if (sigint_received) exit(130);
            if (sigterm_received) exit(143);
            exit(ret);

        default: 
            int status;
            waitpid(pid, &status, 0);
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == 130) {
                    sigint_received = 1;
                    any_signal = 1;
                }
                if (WEXITSTATUS(status) == 143) {
                    any_signal = 1;
                }
                ret = WEXITSTATUS(status);
            }
            return ret;
    }
    return ret;
}

/**
 * @brief Libère la mémoire allouée pour un tableau de commandes.
 *
 * @param cmds Tableau de commandes alloué dynamiquement.
 * @param nb_cmd Nombre de sous-commandes dans le tableau.
 */
void free_cmd(char ***cmds, int nb_cmd) {
    for (int i = 0; i < nb_cmd; i++) {
        free(cmds[i]);
    }
    free(cmds);
}

/**
 * @brief Traite une ligne de commande complète.
 *
 * Cette fonction divise la ligne de commande en sous-commandes basées sur le séparateur `;`,
 * exécute chaque sous-commande séquentiellement, et gère les signaux et la libération des ressources.
 *
 * @param args Tableau des arguments représentant la ligne de commande entière.
 * @return Code de sortie de la dernière commande exécutée ou 1 en cas d'erreur.
 */
int cmd_line(char **args) {
    int nb_cmd = 0;
    char ***cmds = split_cmd(args, &nb_cmd, ";");

    if (cmds == NULL) {
        return 1;
    }

    int ret = 0;
    for (int i = 0; i < nb_cmd; i++) {
        if (sigint_received) {
            free_cmd(cmds, nb_cmd);
            return ret;
        }
        int cmd_size = 0;
        while (cmds[i][cmd_size] != NULL) {
            cmd_size++;
        }
        ret = execute_cmd(cmds[i], cmd_size, ret);
    }

    free_cmd(cmds, nb_cmd);
    return ret;
}
