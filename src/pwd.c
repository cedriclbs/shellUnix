#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
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
        size_t cwd_len = strlen(cwd);
        size_t output_size = cwd_len + 2;
        
        char *output = malloc(output_size);
        if (output == NULL) {
            perror("Erreur d'allocation mémoire");
            return 1;
        }
        snprintf(output, output_size, "%s\n", cwd);

        write(STDOUT_FILENO, output, strlen(output)); 
        return 0;
    } else {
        perror("fsh: erreur\n");
        return 1;
    }
}
