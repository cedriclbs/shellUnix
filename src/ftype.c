#include <stdio.h>
#include <sys/stat.h>
#include "../include/ftype.h"

/**
 * @brief Affiche le type d'un fichier ou d'un répertoire.
 *
 * Cette commande permet de déterminer le type d'un chemin donné et affiche l'une des catégories suivantes :
 * - `directory` : si le chemin correspond à un répertoire.
 * - `regular file` : si le chemin correspond à un fichier régulier.
 * - `symbolic link` : si le chemin est un lien symbolique.
 * - `named pipe` : si le chemin est une FIFO (pipeline nommé).
 * - `other` : pour tout autre type de fichier.
 *
 * @param args Tableau d'arguments. Le deuxième argument (`args[1]`) doit spécifier le chemin du fichier ou du répertoire.
 * @return Un entier représentant le statut de l'exécution :
 *         - 0 si le type du fichier a été déterminé et affiché avec succès.
 *         - 1 en cas d'erreur (ex. argument manquant, chemin invalide, etc.).
 */
int cmd_ftype(char **args) {
    struct stat st;

    if (args[1] == NULL) {
        fprintf(stderr, "ftype : argument manquant\n");
        return 1;
    }

    if (lstat(args[1], &st) < 0) {
        perror("ftype : erreur\n");
        return 1;
    }

    if (S_ISDIR(st.st_mode)) {
        printf("directory\n");
    } else if (S_ISREG(st.st_mode)) {
        printf("regular file\n");
    } else if (S_ISLNK(st.st_mode)) {
        printf("symbolic link\n");
    } else if (S_ISFIFO(st.st_mode)) {
        printf("named pipe\n");
    } else {
        printf("other\n");
    }

    return 0;
}
