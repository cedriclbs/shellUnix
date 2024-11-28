#include <stdio.h>
#include <unistd.h>
#include "../include/pwd.h"

/**
 * @brief Affiche le répertoire de travail actuel.
 *
 * Cette fonction utilise `getcwd()` pour récupérer le répertoire de travail actuel du processus
 * et l'affiche à l'écran. Si une erreur survient lors de la récupération du répertoire,
 * un message d'erreur est affiché.
 *
 * @return 0 Si la commande réussit et le répertoire est affiché avec succès.
 *         1 En cas d'erreur, si `getcwd()` échoue.
 */
int cmd_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("fsh: erreur\n");
        return 1;
    }
}
