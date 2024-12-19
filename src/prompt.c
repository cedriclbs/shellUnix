#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "prompt.h"

#define MAX_LENGTH 30

/**
 * @brief Génère un prompt personnalisé pour le shell, incluant le code de retour, le répertoire actuel et un symbole de prompt.
 *
 * Ce prompt est conçu pour afficher :
 * - Le code de retour du shell, avec une couleur indiquant le succès (vert), l'échec (rouge) ou un signal (rouge avec "SIG").
 * - Le répertoire courant, tronqué si nécessaire pour ne pas dépasser une longueur définie (`MAX_LENGTH`).
 * - Le symbole `$` pour indiquer qu'une commande peut être saisie.
 *
 * Le prompt utilise des codes ANSI pour colorer le texte :
 * - Le code de retour est affiché en vert si `valRes == 0`, en rouge en cas d'échec ou de signal.
 * - Le répertoire est affiché en bleu.
 * 
 * @param valRes Le code de retour précédent du shell (par exemple, le code de retour d'une commande exécutée).
 * @return Une chaîne de caractères allouée dynamiquement représentant le prompt.
 *         Le caller est responsable de la libération de la mémoire de cette chaîne.
 */
char* getPrompt(int valRes) {

    char *prompt = malloc(MAX_LENGTH + 1 + PATH_MAX);

    if (prompt == NULL) {
        perror("Erreur d'allocation memoire");
        exit(1);
    }

    char cwd[PATH_MAX];
    int visible_len = 0;  // Compte les caractères qui vont être affiché
    int len = 0;          // Compte les caractères totaux dans le buffer

    // Basculement des couleurs selon la valeur de retour
    if (valRes == 0) {
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[32m\002[%d]", valRes); // Vert pour succès
        visible_len += snprintf(NULL, 0, "[%d]", valRes);
    } else {
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[91m\002[%d]", valRes); // Rouge pour échec
        visible_len += snprintf(NULL, 0, "[%d]", valRes);
    }

    // Basculement sur la couleur bleue pour le répertoire
    len += snprintf(prompt + len, PATH_MAX - len, "\001\033[34m\002");

    // Obtention du répertoire courant
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("cwd error");
        free(prompt);
        exit(1);
    }

    // Calcule de la longueur restante pour cwd
    int cwd_len = strlen(cwd);
    int max_cwd_len = MAX_LENGTH - visible_len - 2; // 2 pour "$ "

    // Tronquement du chemin si nécessaire
    if (cwd_len > max_cwd_len) {
        len += snprintf(prompt + len, PATH_MAX - len, "...%s", cwd + (cwd_len - max_cwd_len + 3)); // +3 pour les points "..."
        visible_len += max_cwd_len;
    } else {
        len += snprintf(prompt + len, PATH_MAX - len, "%s", cwd);
        visible_len += cwd_len;
    }

    // Basculement sur la couleur normale et ajout du symbole du prompt
    snprintf(prompt + len, PATH_MAX - len, "\001\033[00m\002$ ");

    return prompt;
}
