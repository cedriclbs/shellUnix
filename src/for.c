#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "../include/for.h"
#include "../include/signals.h"
#include "../include/builtins.h"


#define MAX_CMD_ARGS 256
#define MAX_PATH 1024


/**
 * Exécute une boucle for sur le contenu d'un répertoire spécifié.
 * 
 * La fonction attend des arguments similaires à une boucle for d'un shell avec la syntaxe :
 * for variable in répertoire { commande }
 * La commande peut référencer l'élément du répertoire avec $F qui sera remplacé par le nom de chaque fichier/dossier.
 *
 * @param args Un tableau de chaînes de caractères contenant les mots de la commande.
 * @param val Une valeur de contrôle utilisée lors de l'appel de la fonction execute_builtin, peut être utilisée pour le contrôle du flux ou le débogage.
 * @return 0 en cas de succès, 1 en cas d'échec avec un message d'erreur.
 */

/* Structure correspondant aux options de la boucles for */
typedef struct {
    int recursiveOn;
    int hiddenOn;
    int parallelOn;
    char *extension;
    char *type;
} Options;

/**
 * Vérifie si un fichier possède une extension donnée.
 *
 * @param filename Nom du fichier à vérifier.
 * @param extension Extension à comparer.
 * @return 1 si le fichier possède l'extension spécifiée, 0 sinon.
 */
int has_extension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot + 1, extension) == 0);
}

/**
 * Supprime l'extension d'un nom de fichier.
 *
 * @param filename Nom du fichier à modifier.
 */
void remove_extension(char *filename) {
     const char *dot = strrchr(filename, '.');
    size_t len = dot - filename;
    memmove(filename, filename, len);
    filename[len] = '\0';
}

/**
 * Vérifie si un fichier correspond à un type spécifique.
 *
 * @param path Chemin du fichier.
 * @param type Type attendu (`d`, `f`, `l`, etc.).
 * @return 1 si le fichier correspond au type spécifié, 0 sinon.
 */
int is_type(const char *path, char *type) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Erreur stat");
        return 0;
    }
    if (strcmp(type,"d") == 0) return S_ISDIR(st.st_mode);   // répertoire 
    if (strcmp(type,"f") == 0)  return S_ISREG(st.st_mode);   // fichier ordinaire 
    if (strcmp(type,"l") == 0)  return S_ISLNK(st.st_mode);   // Lien symbolique 
    if (strcmp(type,"p") == 0)  return S_ISFIFO(st.st_mode);  // Tube nommé 
    return 0;
}

/**
 * Remplace les occurrences d'une variable (par ex., `$D`) dans une chaîne par une valeur donnée.
 *
 * @param arg Chaîne contenant la variable à remplacer.
 * @param var_name Nom de la variable (ex. : `$D`).
 * @param replacement Valeur à insérer à la place de la variable.
 * @param result Chaîne résultante après remplacement.
 */
void replace_variable(const char *arg, const char *var_name, const char *replacement, char *result) {
    const char *src = arg;
    char *dest = result;
    char *pos;

    while ((pos = strstr(src, var_name)) != NULL) {
        size_t len = pos - src;
        memmove(dest, src, len); // Copier la partie avant la variable
        dest += len;
        memmove(dest, replacement, strlen(replacement)); // Copier la chaîne de remplacement
        dest += strlen(replacement);
        src = pos + strlen(var_name); // Avancer après la variable et chercher la prochaine occurrence
    }
    // Copier le reste de la chaîne
    memmove(dest, src, strlen(src) + 1);  // Inclure le caractère nul '\0'
}

/**
 * Construit et exécute une commande pour un fichier donné.
 *
 * @param filepath Chemin du fichier à utiliser pour la commande.
 * @param var_name Nom de la variable à remplacer dans la commande.
 * @param args Arguments de la commande `for`.
 * @param cmd_start Indice du début de la commande entre accolades.
 * @param cmd_end Indice de fin de la commande entre accolades.
 * @param val_retour Référence vers la valeur de retour globale.
 * @param val Valeur initiale passée à la commande interne.
 */
void executeCmd(const char *filepath, const char *var_name, char **args, int cmd_start, int cmd_end, int *val_retour, int val) {
    char command[MAX_CMD_ARGS][MAX_PATH];  // Tableau statique pour contenir les arguments de la commande
    int cmd_index = 0;

    for (int i = cmd_start; i < cmd_end; i++) {
        char *arg = args[i];
        if (strstr(arg, "$") && strstr(arg, var_name)) {
            char var_fullname[128];
            snprintf(var_fullname, sizeof(var_fullname), "$%s", var_name);
            // Remplacer la variable dans l'argument et remplir command[cmd_index]
            replace_variable(arg, var_fullname, filepath, command[cmd_index]);
        } else {
            // Si l'argument ne contient pas la variable, on copie directement
            snprintf(command[cmd_index], MAX_PATH, "%s", arg);  
        }
        cmd_index++;
    }

    command[cmd_index][0] = '\0';  // S'assurerque la chaîne est bien terminée par un caractère nul

    // Maintenant, on crée un tableau pour passer à `execute_builtin`
    char *final_args[MAX_CMD_ARGS];
    for (int i = 0; i < cmd_index; i++) {
        final_args[i] = command[i];  
    }
    final_args[cmd_index] = NULL; 

    int ret = execute_builtin(final_args, cmd_index, val);  // Exécuter la commande
    if (ret > *val_retour) *val_retour = ret;
}

/**
 * Exécute une commande en mode parallèle.
 *
 * @param filepath Chemin du fichier.
 * @param var_name Nom de la variable à remplacer.
 * @param args Arguments de la commande `for`.
 * @param cmd_start Indice de début de la commande.
 * @param cmd_end Indice de fin de la commande.
 * @param val_retour Référence vers la valeur de retour globale.
 * @param val Valeur initiale.
 * @param max Nombre maximal de processus parallèles autorisés.
 * @param nbOngoing Référence vers le compteur des processus en cours.
 */
void executeCmdWithParallel(const char *filepath, const char *var_name, char **args, int cmd_start, int cmd_end, int *val_retour, int val, int max ,int *nbOngoing){
    if (sigint_received) {
        return; 
    }
    // Attendre qu'un processus ait fini
    while(*nbOngoing >= max){ 
        int status;
        pid_t ended = waitpid(-1,&status,0);
        if(ended >0){
          (*nbOngoing)--;
            if (WIFEXITED(status)) {
                int child_ret = WEXITSTATUS(status);
                if (child_ret > *val_retour) {
                    *val_retour = child_ret;
                }
            }
        }
    }
    pid_t pid;

    switch(pid = fork()){
        case -1 : 
        perror("for: Erreur sur le fork");
        *val_retour =1;
        return;

        case 0 : // processus fils qui exécute la commande
        executeCmd(filepath, var_name, args, cmd_start, cmd_end, val_retour, val);
        exit(*val_retour);

        default :
        (*nbOngoing)++; 
    }
}

/**
 * Exécute récursivement une boucle `for` sur un répertoire.
 *
 * @param directory Répertoire à parcourir.
 * @param var_name Nom de la variable à remplacer dans les commandes.
 * @param args Arguments de la commande `for`.
 * @param cmd_start Indice de début de la commande entre accolades.
 * @param cmd_end Indice de fin de la commande entre accolades.
 * @param options Options de la boucle `for`.
 * @param val Valeur initiale pour l'exécution.
 * @param val_retour Référence vers la valeur de retour globale.
 * @param nbOngoing Référence vers le compteur des processus en cours.
 */
void for_rec(const char *directory, const char *var_name, char **args, int cmd_start, int cmd_end, Options *options, int val, int *val_retour, int *nbOngoing) {
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Erreur d'ouverture du répertoire");
        *val_retour = -1;
        return;
    }

    struct dirent *entry;
    char filepath[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        // Ignore les dossiers . et ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        // Ignore les fichiers cachés si l'option -A n'est pas activée
        if (!options->hiddenOn && entry->d_name[0] == '.') continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
        if (is_type(filepath, "l")) continue;

        if (options->type && !is_type(filepath, options->type)) {
            if (is_type(filepath, "d") && options->recursiveOn) {
                for_rec(filepath, var_name, args, cmd_start, cmd_end, options, val, val_retour,nbOngoing);
            }
            continue;
        }

        char *nameEntry = entry->d_name;
        int removed =0;
        if (options->extension) {
            if (!has_extension(nameEntry, options->extension)) {
                if (is_type(filepath, "d") && options->recursiveOn) for_rec(filepath, var_name, args, cmd_start, cmd_end, options, val, val_retour,nbOngoing);
                continue;
            } else {
                remove_extension(nameEntry);
                removed=1;
            }
        }

        char *path=filepath;
        if (removed){
            char filepath2[MAX_PATH];
            snprintf(filepath2, sizeof(filepath), "%s/%s", directory, nameEntry);
            path= filepath2;
        }

        // Gère la parallélisation
        if (options->parallelOn > 0) {
            executeCmdWithParallel(path, var_name, args, cmd_start, cmd_end, val_retour, val, options->parallelOn, nbOngoing);
        } else {
            executeCmd(path, var_name, args, cmd_start, cmd_end, val_retour, val);
        }

        if (is_type(filepath, "d") && options->recursiveOn) {
            for_rec(filepath, var_name, args, cmd_start, cmd_end, options, val, val_retour,nbOngoing);
        }
    }
    closedir(dir);
}

/**
 * Vérifie si la boucle `for` ne contient aucune option active.
 *
 * @param options Pointeur vers la structure des options.
 * @return 1 si aucune option n'est activée, 0 sinon.
 */
int isNotOptions(Options *options) {
    return options->recursiveOn == 0 && options->hiddenOn == 0 && options->parallelOn == 0
           && options->extension == NULL && options->type == NULL;
}

/**
 * Implémente la commande interne `for`.
 *
 * Syntaxe :
 * ```
 * for <variable> in <répertoire> [options] { <commande> }
 * ```
 * - `variable` : Nom de la variable utilisée dans la commande.
 * - `répertoire` : Chemin du répertoire ou des éléments à parcourir.
 * - `[options]` : Options disponibles (ex. : `-r`, `-e`, `-t`, `-p`, etc.).
 * - `<commande>` : Commande exécutée pour chaque élément, entre accolades `{}`.
 *
 * Options :
 * - `-r` : Active la récursivité.
 * - `-A` : Inclut les fichiers/dossiers cachés.
 * - `-e <extension>` : Filtre les fichiers par extension.
 * - `-t <type>` : Filtre par type (ex. : `d` pour répertoires, `f` pour fichiers).
 * - `-p <n>` : Exécution en parallèle avec `n` processus simultanés.
 *
 * @param args Tableau des arguments de la commande `for`.
 * @param argc Nombre total d'arguments dans `args`.
 * @param val Valeur initiale utilisée pour les commandes internes.
 * @return Code de retour :
 *         - 0 si la boucle s'est exécutée avec succès.
 *         - 1 en cas d'erreur (syntaxe, accès répertoire, etc.).
 *         - 2 si la syntaxe de la commande est incorrecte.
 */
int cmd_for(char **args, int argc, int val) {
    if (!args || strcmp(args[0], "for") != 0 || !args[1] || strcmp(args[2], "in") != 0 || !args[3]) {
        perror("for: Syntaxe incorrecte pour la boucle 'for'");
        return 2;
    }

    char *var_name = args[1];  // Nom de la variable (ex: D ou F)
    char *directory = args[3]; // Répertoire ou liste d'éléments
    Options options = {0, 0, 0,NULL, NULL};
    int val_retour = 0;

    // Chercher le bloc de commadnes entre { et }
    int cmd_size = 0;
    int cmd_start = -1, cmd_end = -1, brace_count = 0;

    for (int i = 4; args[i] != NULL; i++) {
        if (strcmp(args[i], "{") == 0) {
            if (brace_count == 0) cmd_start = i + 1;
            if (brace_count > 0) cmd_size++;
            brace_count++;
        } else if (strcmp(args[i], "}") == 0) {
            brace_count--;
            if (brace_count == 0) cmd_end = i;
            if (brace_count > 0) cmd_size++;
        } else if (strcmp(args[i], "-A") == 0) {
            options.hiddenOn = 1;
        } else if (strcmp(args[i], "-e") == 0 && brace_count == 0) {
            if (args[++i]) {
                options.extension = args[i];
            } else {
                perror("for: -e a besoin d'un argument");
                return 1;
            }
        } else if (strcmp(args[i], "-r") == 0 && brace_count == 0) {
            options.recursiveOn = 1;
        } else if (strcmp(args[i], "-t") == 0 && brace_count == 0) {
            if (args[++i]) {
                options.type = args[i];
            } else {
                perror("for: -t a besoin d'un argument");
                return 1;
            }
        } else if (strcmp(args[i], "-p") == 0 && brace_count == 0) {
            if (args[++i]) {
                int is_number = 1; // Indicateur pour vérifier si tous les caractères sont des chiffres
                for (int j = 0; args[i][j] != '\0'; j++) {
                    if (!isdigit(args[i][j])) {
                        is_number = 0;
                        break;
                    }
                }

                if (!is_number) {
                    perror("for: l'argument de -p doit être un nombre");
                    return 1;
                }else{
                    if(atoi(args[i])<=0){
                        perror("for: l'argument de -p doit être supérieur à 0");
                        return 1;
                    } else {
                        options.parallelOn = atoi(args[i]);
                    }
                }
            } else {
                perror("for: -p a besoin d'un argument");
                return 1;
            }
        } else {
            if (brace_count == 0) {
                perror("for: Option non reconnue");
                printf("OPTION NON RECONNU%s", args[i]);
                return 1;
            }
            cmd_size++;
        }
    }

    // Accolades bien placées ?
    int isOptionNotOn = isNotOptions(&options);
    if (cmd_start == -1 || cmd_end == -1 || brace_count != 0
        || (isOptionNotOn && strcmp(args[4], "{") != 0)
        || (isOptionNotOn && argc - cmd_size!= 6)) {
        perror("for: Erreur de syntaxe");
        return 2;
    }

    int nbOngoing=0; // nombre de processus en cours
    for_rec(directory, var_name, args, cmd_start, cmd_end, &options, val, &val_retour,&nbOngoing);

    //Tous les processus enfants doivent être terminé 
    while (options.parallelOn > 0 && nbOngoing > 0) {
/*         printf("Attente des processus restants (nbOngoing = %d)\n", nbOngoing);
 */        int status;
        pid_t finished = waitpid(-1, &status, 0);
        if (finished > 0) {
            nbOngoing--;
        }
    }

    if(val_retour==-1) return 1; // REP invalide
    return val_retour;
}
