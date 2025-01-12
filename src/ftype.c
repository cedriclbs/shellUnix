#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/ftype.h"

/**
 * @brief Affiche le type d'un fichier ou d'un répertoire spécifié.
 *
 * Cette commande permet de déterminer le type d'un chemin donné et affiche l'une des catégories suivantes :
 * - `directory` : si le chemin correspond à un répertoire.
 * - `regular file` : si le chemin correspond à un fichier régulier.
 * - `symbolic link` : si le chemin est un lien symbolique.
 * - `named pipe` : si le chemin est une FIFO (pipeline nommé).
 * - `other` : pour tout autre type de fichier ou chemin non catégorisé.
 *
 * ### Fonctionnement :
 * - La fonction utilise `lstat` pour obtenir les métadonnées associées au chemin donné.
 * - Les résultats sont affichés sur la sortie standard (`STDOUT`) sous forme d'une chaîne.
 * - En cas d'erreur (par exemple, chemin invalide ou argument manquant), un message
 *   d'erreur est affiché sur la sortie d'erreur (`STDERR`).
 * @param args Tableau de chaînes représentant les arguments de la commande.
 *             - `args[0]` : la commande elle-même (`ftype`).
 *             - `args[1]` : chemin du fichier ou répertoire à analyser.
 *
 * @return Un entier représentant le statut d'exécution :
 *         - 0 si le type du fichier a été déterminé et affiché avec succès.
 *         - 1 en cas d'erreur (par exemple, argument manquant ou chemin invalide).
 */
int cmd_ftype(char **args) {
    struct stat st;
    if (args[1] == NULL) {
        perror("ftype : argument manquant");
        return 1;
    }
    if (lstat(args[1], &st) < 0) {
        perror("ftype : erreur\n");
        return 1;
    }
    if (S_ISDIR(st.st_mode)) {
        const char *message = "directory\n";
        write(STDOUT_FILENO, message, strlen(message)); // Écrit sur STDOUT
    } else if (S_ISREG(st.st_mode)) {
        const char *message = "regular file\n";
        write(STDOUT_FILENO, message, strlen(message)); // Écrit sur STDOUT
    } else if (S_ISLNK(st.st_mode)) {
        const char *message = "symbolic link\n";
        write(STDOUT_FILENO, message, strlen(message)); // Écrit sur STDOUT
    } else if (S_ISFIFO(st.st_mode)) {
        const char *message = "named pipe\n";
        write(STDOUT_FILENO, message, strlen(message)); // Écrit sur STDOUT
    } else {
        const char *message = "other\n";
        write(STDOUT_FILENO, message, strlen(message)); // Écrit sur STDOUT
    }
    return 0;
}
