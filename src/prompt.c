#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <readline/history.h>
#include "prompt.h"
#include <stdbool.h>
#include <signal.h>
#include "signals.h"


#define MAX_LENGTH 30

/**
 * @brief Génère un prompt coloré et informatif pour le shell.
 *
 * Ce prompt est conçu pour afficher :
 * - **Code de retour** : En vert (succès), rouge (échec), ou rouge avec `[SIG]` en cas de signal.
 * - **Répertoire courant** : En bleu. Si le chemin dépasse une certaine longueur, il est tronqué avec `...`.
 * - **Symbole `$`** : Indique la possibilité de saisir une commande.
 *
 * ### Fonctionnalités :
 * - Utilise des codes ANSI pour la couleur, compatibles avec la bibliothèque `readline`.
 * - Tronque le répertoire courant si la longueur totale dépasse `MAX_LENGTH`.
 * - Gère les signaux en affichant `[SIG]` en cas d'interruption ou terminaison due à un signal.
 * @param valRes Le code de retour précédent du shell (par exemple, le code de retour d'une commande exécutée).
 *               - `0` pour succès.
 *               - `128+signal_number` si une commande a été interrompue par un signal.
 *               - Autres valeurs pour des erreurs.
 *
 * @return Une chaîne de caractères allouée dynamiquement représentant le prompt.
 *         Le programme appelant est responsable de libérer la mémoire de cette chaîne avec `free`.
 */
char* getPrompt(int valRes) {

    char *prompt = malloc(MAX_LENGTH + 1 + PATH_MAX);

    if (prompt == NULL) {
        perror("Erreur d'allocation memoire");
        clear_history();
        exit(1);
    }

    char cwd[PATH_MAX];
    int visible_len = 0; 
    int len = 0;          


    // Basculement des couleurs selon la valeur de retour
    if (valRes == 255) {
        // Code de retour normal (255) sans signal
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[91m\002[%d]", valRes);
        visible_len += snprintf(NULL, 0, "[%d]", valRes);
    } 
    else if ((valRes >= 128 && any_signal )) {
        //Signal -> valeur par défaut [SIG]
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[91m\002[SIG]");
        visible_len += snprintf(NULL, 0, "[SIG]");
    }
    
    else if (valRes >= 0) {
        // Vert pour succès
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[32m\002[%d]", valRes);
        visible_len += snprintf(NULL, 0, "[%d]", valRes);
    }
    
    else {
        // Rouge pour échec
        len += snprintf(prompt + len, PATH_MAX - len, "\001\033[91m\002[%d]", valRes);
        visible_len += snprintf(NULL, 0, "[%d]", valRes);
    }




    // Basculement sur la couleur bleue pour le répertoire
    len += snprintf(prompt + len, PATH_MAX - len, "\001\033[34m\002");

    // Obtention du répertoire courant
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("cwd error");
        free(prompt);
        clear_history();
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
    resetSigs();
    return prompt;
}
