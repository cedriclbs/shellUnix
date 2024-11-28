#include <stdlib.h>
#include <ctype.h>
#include <stdio.h> 
#include "../include/exit.h"

/**
 * @brief Implémente la commande interne `exit` pour terminer le programme.
 *
 * Cette fonction permet de quitter le programme avec un code de retour spécifié.
 * Si un argument est fourni, il doit être un entier valide. Sinon, un message
 * d'erreur est affiché, et le programme termine avec un code d'erreur par défaut (255).
 * 
 * Cas d'utilisation :
 * - `exit` : termine avec le dernier code de retour (valeur par défaut `val`).
 * - `exit <code>` : termine avec le code de retour spécifié (`<code>` doit être un entier valide).
 *
 * @param args Tableau d'arguments de la commande. 
 *             `args[0]` est la commande elle-même, et `args[1]` (optionnel) est le code de retour.
 * @param val  Dernier code de retour connu à utiliser si aucun argument n'est fourni.
 * @return Cette fonction ne retourne jamais car elle appelle `exit()` pour quitter le programme.
 */
int cmd_exit(char **args, int val) {
    int ret = val;

    // Vérifie si un argument est passé
    if (args[1] != NULL) {
        for (int i = 0; args[1][i] != '\0'; i++) {
            if (!isdigit(args[1][i])) {
                fprintf(stderr, "exit: %s: argument numerique requis\n", args[1]);
                exit(255); 
            }
        }
        ret = atoi(args[1]);
    }
    exit(ret); 
}
