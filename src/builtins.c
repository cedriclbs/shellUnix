#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "../include/builtins.h"


int cmd_cd (char **args){
    static char prev_dir[1024] = "";
    char current_dir[1024];
    char* path_h;


    // "cd" -> répertoire HOME
    if (args[1]==NULL){
        path_h = getenv("HOME");
        if (path_h == NULL){
            fprintf(stderr, "fsh: impossible de trouver le répertoire HOME\n");
            return 1;
        }
        else if (chdir(path_h)){
            perror("fsh: répertoire HOME non trouvé\n");
            return 1;
        }
    }

    // "cd -" -> répertoire précédent
    else if (strcmp(args[1], "-") == 0){
        if (strlen(prev_dir)==0){
            fprintf(stderr, "fsh: répertoire précédent inexistant\n");
            return 1;
        }
        if (chdir(prev_dir)!=0){
            perror("fsh: erreur\n");
            return 1;
        }
        printf("%s\n", prev_dir);
    }

    // "cd arg"
    else {
        if (chdir(args[1]) != 0){
            perror("fsh: chemin invalide\n");
            return 1;
        }
    }

    // Mettre à jour prev_dir pour cd -
    if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
        strncpy(prev_dir, current_dir, sizeof(prev_dir) - 1);
    } else {
        perror("fsh");
    }

    return 0;

}




int cmd_exit (char **args){
    int val = 0;

    // Exit avec la valeur VAL
    if (args[1] != NULL){
        val = atoi(args[1]);
    }

    exit(val);

}



int cmd_pwd () {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL && cwd != NULL){
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("fsh: erreur\n");
        return 1;
    }
}


int cmd_ftype (char **args) {

    struct stat st;

    if (args[1] == NULL){
        fprintf(stderr, "ftype : argument manquant\n");
        return 1;
    }

    if (stat(args[1], &st) < 0){
        perror("ftype : erreur\n");
        return 1;
    }


    if (S_ISDIR(st.st_mode)){
        printf("directory\n");
    } else if (S_ISREG(st.st_mode)){
        printf("regular file\n");
    } else if (S_ISLNK(st.st_mode)){
        printf("symbolic link\n");
    } else if (S_ISFIFO(st.st_mode)){
        printf("named pipe\n");
    } else {
        printf("other\n");
    }


    return 0;


}