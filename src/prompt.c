#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "prompt.h"

#define MAX_LENGTH 30

char* getPrompt(int valRes) {

    char *prompt = malloc(MAX_LENGTH + 1 + PATH_MAX +10);

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
    } else if (valRes > 128) {
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[91m\002[SIG]"); // Rouge pour signal
        visible_len += snprintf(NULL, 0, "[SIG]");
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

    // Calcul de la longueur restante pour cwd
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
