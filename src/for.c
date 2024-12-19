#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/for.h"
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


// Vérifie si un fichier a une extension donnée
int has_extension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot + 1, extension) == 0);
}

// Retire l'extension d'un nom de fichier
void remove_extension(char *filename) {
    const char *dot = strrchr(filename, '.');
    size_t len = dot - filename;
    memmove(filename, filename, len);
    filename[len] = '\0';
}

//Fonction vérifiant le type de fichier selon le type donné en paramètre
int is_type(const char *path, char *type) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Erreur stat");
        return 0;
    }
    if (strcmp(type,"d") == 0) return S_ISDIR(st.st_mode);   // répertoire ?
    if (strcmp(type,"f") == 0)  return S_ISREG(st.st_mode);   // fichier ordinaire ?
    if (strcmp(type,"l") == 0)  return S_ISLNK(st.st_mode);   // Lien symbolique ?
    if (strcmp(type,"p") == 0)  return S_ISFIFO(st.st_mode);  // Tube nommé ?
    return 0;
}


// Fonction pour remplacer les variables dynamiques comme "$D" dans les arguments
void replace_variable(const char *arg, const char *var_name, const char *replacement, char *result) {
    const char *src = arg;
    char *dest = result;
    char *pos;

    while ((pos = strstr(src, var_name)) != NULL) {
        size_t len = pos - src;
        // Copie la partie avant la variable
        memmove(dest, src, len);
        dest += len;

        // Copie la chaîne de remplacement
        memmove(dest, replacement, strlen(replacement));
        dest += strlen(replacement);

        // Avance après la variable et chercher la prochaine occurrence
        src = pos + strlen(var_name);
    }

    // Copier le reste de la chaîne
    memmove(dest, src, strlen(src) + 1);  // Inclure le caractère nul '\0'
}




// Fonction construisant et exécutant une commande pour un fichier donné en arg
void executeCmd(const char *filepath, const char *var_name, char **args, int cmd_start, int cmd_end, int *val_retour, int val) {
    char command[MAX_CMD_ARGS][MAX_PATH];  // Tableau statique pour contenir les arguments de la commande
    int cmd_index = 0;

    for (int i = cmd_start; i < cmd_end; i++) {
        char *arg = args[i];
        if (strstr(arg, "$") && strstr(arg, var_name)) {
            char var_fullname[128];
            snprintf(var_fullname, sizeof(var_fullname), "$%s", var_name);

            // Remplace la variable dans l'argument et remplir command[cmd_index]
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

    int ret = execute_builtin(final_args, cmd_index, val);  
    if (ret > *val_retour) {
        *val_retour = ret;
    }
}



// Fonction récursive si l'option -R est activée
void for_rec(const char *directory, const char *var_name, char **args, int cmd_start, int cmd_end, const char *extension, int hiddenOn, char *type, int val, int *val_retour) {
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Erreur d'ouverture du répertoire");
        return;
    }
    struct dirent *entry;
    char filepath[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {

        // Ignore les dossiers . et ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Ignore les fichiers cachés si l'option -A n'est pas activée
        if (!hiddenOn && entry->d_name[0] == '.') {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);



        if (type && !is_type(filepath, type)) {
            if (is_type(filepath, "d")) {
                for_rec(filepath, var_name, args, cmd_start, cmd_end, extension, hiddenOn, type, val, val_retour);
            }
            continue;
        }

        char *nameEntry = entry->d_name;
        if (extension && !has_extension(nameEntry, extension)) {
            if (is_type(filepath, "d")) {
                for_rec(filepath, var_name, args, cmd_start, cmd_end, extension, hiddenOn, type, val, val_retour);
            }
            continue;
        }
        if (extension){
            remove_extension(nameEntry);
        } 


        executeCmd(filepath, var_name, args, cmd_start, cmd_end, val_retour, val);


        if (is_type(filepath, "d")) {
            for_rec(filepath, var_name, args, cmd_start, cmd_end, extension, hiddenOn, type, val, val_retour);
        }


    }

    closedir(dir);

}

// Fonction principale
int cmd_for(char **args,int argc, int val) {

    if (!args || strcmp(args[0], "for") != 0 || !args[1] || strcmp(args[2], "in") != 0 || !args[3]) {
        perror("Erreur : Syntaxe incorrecte pour la boucle 'for'");
        return 2;
    }

    char *var_name = args[1];        // Nom de la variable (ex: D ou F)
    char *directory = args[3];       // Répertoire ou liste d'éléments
    
    const char *extension = NULL;
    char *type = NULL;
    int recursiveOn = 0;
    int hiddenOn = 0;
    int typeOn = 0;
    int val_retour = 0;



    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Erreur d'ouverture du répertoire");
        return 1;
    }

    int cmd_size =0;

    // Cherche le bloc de commandes entre { et }
    int cmd_start = -1, cmd_end = -1, brace_count = 0;
    for (int i = 4; args[i] != NULL; i++) {
        if (strcmp(args[i], "{") == 0) {
            if (brace_count == 0) cmd_start = i + 1;
            if(brace_count!=0){
                cmd_size++;
            }
            brace_count++;
        } else if (strcmp(args[i], "}") == 0) {
            brace_count--;
            if (brace_count == 0) {
                cmd_end = i;
            }
            if(brace_count!=0){
                cmd_size++;
            }
        } else if (strcmp(args[i], "-A") == 0){
            hiddenOn = 1;
        }  else if (strcmp(args[i], "-e") == 0 && brace_count == 0){
            if (args[++i]) {
                extension = args[i];
            } else {
                perror("Erreur : -e a besoin d'un argument");
                closedir(dir);
                return 1;
            }
            //printf("CED TEST %s", args[i+1]);
        } else if (strcmp(args[i], "-r") == 0 && brace_count == 0) {
            recursiveOn = 1;
        } else if (strcmp(args[i], "-t") == 0 && brace_count == 0) {
            if (args[++i]){
                type = args[i];
                typeOn = 1;
            } else {
                perror("Erreur : -t a besoin d'un argument");
                closedir(dir);
                return 1;
            }
        }else{
            if(brace_count == 0){
                perror("for: Option non reconnue");
                return 1;
            }
            cmd_size++;
        }
    }

    // Accolades bien placées ?
    int isOptionNotOn = !typeOn && !recursiveOn && !hiddenOn && !extension;
    if (cmd_start == -1 || cmd_end == -1  || brace_count !=0
    || (isOptionNotOn && strcmp(args[4], "{") != 0)
    || (isOptionNotOn && argc-cmd_size != 6) ) {
        perror("for: Erreur de syntaxe");
        closedir(dir);
        return 2;
    }



    if (recursiveOn) {
        for_rec(directory, var_name, args, cmd_start, cmd_end, extension, hiddenOn, type, val, &val_retour);
    } else {
        
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {

            // Ignore les dossiers . et ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            // Ignore les fichiers cachés si l'option -A n'est pas activée
            if (!hiddenOn && entry->d_name[0] == '.') {
                continue;
            }

            char *name = entry->d_name;
            if (extension) {
                if (!has_extension(entry->d_name, extension)) {
                    continue;
                } else {
                    remove_extension(name);
                }
            }

            char filepath[MAX_PATH];
            snprintf(filepath, sizeof(filepath), "%s/%s", directory, name);


            // Si l'option '-t' est spécifiée : vérifie le type de fichier 
            if (typeOn && !is_type(filepath, type)) {
                continue;
            }

            // Construit et exécute la commande
            executeCmd(filepath, var_name, args, cmd_start, cmd_end, &val_retour, val);

        }

    }
    closedir(dir);
    return val_retour;
}
