#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/builtins.h"
#include "../include/execute.h"
#include "../include/cmd_line.h"
#include "../include/ifelse.h"
#include "../include/redirections.h"
#include "../include/pipelines.h"

/**
 * @file execute_builtin.c
 * @brief Fichier contenant les fonctions principales pour l'exécution des commandes internes ou externes
 * avec gestion des redirections et des pipelines.
 *
 * @brief Vérifie la présence d'une sous-chaîne dans les arguments de la commande.
 *
 * Parcourt les arguments donnés pour vérifier si une sous-chaîne spécifique est présente.
 *
 * @param args Tableau de chaînes de caractères contenant les arguments de la commande.
 * @param argc Nombre d'éléments dans le tableau `args`.
 * @param s Sous-chaîne à rechercher dans les arguments.
 * @return 1 si la sous-chaîne est trouvée dans les arguments, sinon 0.
 */
int isIn(char **args, int argc, const char *s) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(args[i], s) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Gère les redirections des entrées et sorties pour les commandes.
 *
 * Traite les arguments pour identifier les redirections (<, >, 2>, etc.)
 * et met à jour les fichiers et descripteurs correspondants.
 *
 * @param args Arguments de la commande.
 * @param cmd_args Référence vers le tableau d'arguments modifiés après le traitement des redirections.
 * @param input_file Référence vers la chaîne qui sera utilisée pour l'entrée redirigée.
 * @param output_file Référence vers la chaîne qui sera utilisée pour la sortie redirigée.
 * @param error_file Référence vers la chaîne qui sera utilisée pour les erreurs redirigées.
 * @param output_flags Référence vers les indicateurs utilisés pour la redirection de sortie.
 * @param error_flags Référence vers les indicateurs utilisés pour la redirection des erreurs.
 * @param cmd_size Référence vers la taille du tableau d'arguments après traitement.
 * @return 0 si les redirections ont été traitées avec succès, sinon 1.
 */
int handle_redirections(char **args, char ***cmd_args, char **input_file, char **output_file, char **error_file, int *output_flags, int *error_flags, size_t *cmd_size) {
    *cmd_args = parse_redirections(args, input_file, output_file, error_file, output_flags, error_flags, cmd_size);
    if (*cmd_args == NULL) {
        return 1;
    }
    return 0;
}

/**
 * @brief Restaure les descripteurs de fichiers standard après une redirection.
 *
 * Rétablit les descripteurs de fichiers standard (STDIN, STDOUT, STDERR)
 * après une redirection temporaire.
 *
 * @param stdin_copy Copie du descripteur de fichier d'entrée standard.
 * @param stdout_copy Copie du descripteur de fichier de sortie standard.
 * @param stderr_copy Copie du descripteur de fichier d'erreurs standard.
 * @return Toujours 0, indiquant que la restauration a été effectuée.
 */
int restore_descriptors(int stdin_copy, int stdout_copy, int stderr_copy) {
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    dup2(stderr_copy, STDERR_FILENO);
    close(stdin_copy);
    close(stdout_copy);
    close(stderr_copy);
    return 0;
}

/**
 * @brief Exécute une commande en gérant les erreurs de syntaxe et de redirection.
 *
 * Identifie et exécute les commandes internes (comme `cd`, `exit`, `pwd`)
 * ou les passe à une fonction d'exécution standard avec redirections.
 *
 * @param cmd_args Tableau des arguments de la commande.
 * @param cmd_size Taille du tableau d'arguments.
 * @param val Valeur à utiliser pour certaines commandes internes (ex. `exit` ou `for`).
 * @return Le code de retour de la commande exécutée.
 */
int execute_command(char **cmd_args, int cmd_size, int val) {
    int result = 0;
    if (strcmp(cmd_args[0], "cd") == 0) {
        if (cmd_size > 2) {
            perror("cd: trop d'arguments");
            result = 1;
        } else {
            result = cmd_cd(cmd_args);
        }
    } else if (strcmp(cmd_args[0], "exit") == 0) {
        if (cmd_size > 2) {
            perror("exit: trop d'arguments");
            result = 1;
        } else {
            result = cmd_exit(cmd_args, val);
        }
    } else if (strcmp(cmd_args[0], "pwd") == 0) {
        if (cmd_size > 1) {
            perror("pwd: trop d'arguments");
            result = 1;
        } else {
            result = cmd_pwd();
        }
    } else if (strcmp(cmd_args[0], "ftype") == 0) {
        result = cmd_ftype(cmd_args);
    } else {
        result = execute_command_with_redirection(cmd_args);
    }
    return result;
}

/**
 * @brief Vérifie la présence d'un délimiteur en dehors des accolades.
 *
 * Parcourt les arguments pour vérifier si un délimiteur donné
 * (comme `;` ou `|`) est présent en dehors des paires d'accolades `{}`.
 *
 * @param args Tableau de chaînes représentant les arguments de la commande.
 * @param delimiter Chaîne représentant le délimiteur à rechercher.
 * @return 1 si le délimiteur est trouvé en dehors des accolades, sinon 0.
 */
int isThereDelimiterOutside(char **args, char *delimiter) {
    int in_braces = 0;
    int isDelimiterOutside = 0;
    int i = 0;

    while (args[i] != NULL) {
        if (strcmp(args[i], "{") == 0) {
            in_braces++; // Entrée dans une paire d'accolades
        } else if (strcmp(args[i], "}") == 0) {
            in_braces--;  // Sortie de la paire d'accolades
        } else if (in_braces == 0 && strcmp(args[i], delimiter) == 0) {
            isDelimiterOutside = 1;
        }
        i++;
    }

    return isDelimiterOutside;
}

/**
 * @brief Exécute une commande interne ou externe avec gestion des redirections et des délimiteurs.
 *
 * Cette fonction identifie les délimiteurs (`;`, `|`),
 * exécute les commandes internes (`for`, `if`, etc.),
 * ou gère les redirections pour les commandes externes.
 *
 * @param args Tableau de chaînes représentant les arguments de la commande.
 * @param argc Nombre total d'arguments.
 * @param val Dernier code de retour connu, utilisé pour certaines commandes internes.
 * @return Le code de retour de la commande exécutée.
 */
int execute_builtin(char **args, int argc, int val) {
    if (args[0] == NULL) {
        return val;
    }
    if (isThereDelimiterOutside(args, ";") == 1) {
        return cmd_line(args);
    }
    else if (isTherePipeOutside(args) == 1) {
        return cmd_pipeline(args, val);
    }
    else if (strcmp(args[0], "for") == 0) {
        return cmd_for(args, argc, val);
    } else if (strcmp(args[0], "if") == 0) {
        return cmd_if(args, val);
    } else {
        char *input_file = NULL, *output_file = NULL, *error_file = NULL;
        int output_flags = 0, error_flags = 0;
        size_t cmd_size = argc;
        char **cmd_args;

        if (handle_redirections(args, &cmd_args, &input_file, &output_file, &error_file,
                                &output_flags, &error_flags, &cmd_size) != 0) {
            free(cmd_args);
            return 1;
        }
        int saved_stdin = dup(STDIN_FILENO);
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stderr = dup(STDERR_FILENO);

        if (saved_stdin == -1 || saved_stdout == -1 || saved_stderr == -1) {
            perror("dup échoué");
            free(cmd_args);
            return 1;
        }
        if (setup_redirections(input_file, output_file, error_file, output_flags, error_flags) != 0) {
            restore_descriptors(saved_stdin, saved_stdout, saved_stderr);
            free(cmd_args);
            return 1;
        }
        int result = execute_command(cmd_args, cmd_size, val);
        restore_descriptors(saved_stdin, saved_stdout, saved_stderr);
        free(cmd_args);
        return result;
    }
}
