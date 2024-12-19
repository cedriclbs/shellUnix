#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"

int cmd_if(char **args, int val) {
    // Partie du if

    // Extraire la pipeline TEST
    char *cmd_args[100];
    int cmd_argc = 0;
    int i = 1;

    while (args[i] && strcmp(args[i], "{") != 0) {
        cmd_args[cmd_argc++] = args[i];
        i++;
    }

    if (cmd_argc == 0 || !args[i] || strcmp(args[i], "{") != 0) {
        return 2;
    }

    cmd_args[cmd_argc] = NULL; // Termine le tableau de TEST
    i++; // Passe après "{"

    // Récupère la commande CMD_2
    char *cmd2_args[100];
    int cmd2_argc = 0;

    int brace_count = 1; // Compte des accolades ouvertes
    while (args[i] && brace_count > 0) {
        if (strcmp(args[i], "{") == 0) {
            brace_count++;
        } else if (strcmp(args[i], "}") == 0) {
            brace_count--;
        }

        if (brace_count > 0) { // Inclure la commande si dans un bloc valide
            cmd2_args[cmd2_argc++] = args[i];
        }
        i++;
    }

    if (brace_count != 0) {
        return 2; // Erreur de syntaxe (accolades déséquilibrées)
    }

    cmd2_args[cmd2_argc] = NULL;

    // Partie du else
    char *cmd3_args[100];
    int cmd3_argc = 0;
    int isElse = 0;

    if (args[i] && strcmp(args[i], "else") == 0) {
        i++; // Passe à "else"
        if (args[i] && strcmp(args[i], "{") == 0) {
            i++; // Passe après "{"

            brace_count = 1; // Réinitialise pour le bloc else
            while (args[i] && brace_count > 0) {
                if (strcmp(args[i], "{") == 0) {
                    brace_count++;
                } else if (strcmp(args[i], "}") == 0) {
                    brace_count--;
                }
                if (brace_count > 0) {
                    cmd3_args[cmd3_argc++] = args[i];
                }
                i++;
            }

            if (brace_count != 0) {
                return 2;
            }

            cmd3_args[cmd3_argc] = NULL;
            isElse = 1;

            // Vérification de la fin
            if (args[i] != NULL) {
                return 2; 
            }
        } else {
            perror("if: Erreur de syntaxe");
            return 2; 
        }
    }

    // Exécute TEST
    int ret = execute_builtin(cmd_args, cmd_argc, val);

    if (ret == 0) {
        int cmd2_ret = execute_builtin(cmd2_args, cmd2_argc, val);
        if (cmd2_ret != 0) {
            return cmd2_ret;
        }
    } else if (isElse) {
        int cmd3_ret = execute_builtin(cmd3_args, cmd3_argc, val);
        if (cmd3_ret != 0) {
            return cmd3_ret;
        }
    }

    return 0;
}
