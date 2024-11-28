#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/builtins.h"
#include "../include/execute.h"

/**
 * @brief Exécute une commande interne ou externe.
 *
 * Cette fonction détermine si une commande saisie par l'utilisateur est une
 * commande interne (comme `cd`, `exit`, `pwd`, etc.) ou une commande externe,
 * puis l'exécute en conséquence.
 *
 * @param args Tableau de chaînes de caractères représentant les arguments
 *             de la commande. args[0] contient la commande elle-même.
 * @param val  Valeur passée à certaines commandes (ex. boucle `for` ou `exit`).
 * @return Un entier représentant le code de retour de la commande exécutée :
 *         - Si la commande est interne et réussit, la valeur dépend de l'implémentation.
 *         - Si la commande est externe, retourne le code de retour du processus exécuté.
 *         - Si une erreur survient, retourne une valeur négative (ex. -1).
 */
int execute_builtin(char **args, int val) {
    if (args[0] == NULL) {
        return val; 
    }
    if (strcmp(args[0], "cd") == 0) {
        return cmd_cd(args);
    } else if (strcmp(args[0], "exit") == 0) {
        return cmd_exit(args, val);
    } else if (strcmp(args[0], "pwd") == 0) {
        return cmd_pwd();
    } else if (strcmp(args[0], "ftype") == 0) {
        return cmd_ftype(args);
    } else if (strcmp(args[0], "for") == 0) {
        return cmd_for(args, val);
    } else {
        int result = execute_command_with_redirection(args);
        if (result == -1) {
            perror("Commande inconnue");
        }
        return result;
    }
}
