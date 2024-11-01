#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <readline/readline.h>

#define MAX_LENGTH 30

void printPrompt(int valRes) {

    char prompt[MAX_LENGTH + 1];
    char cwd[PATH_MAX];
    int len = 0;

    //Basculement de couleur + valeur de retour entre crochets
    if(valRes == 0) {
        sprintf(prompt + len, "\001\033[32m\002[%d]", valRes); // Commande réussi donc vert
    }
    else if (valRes > 128) {
        sprintf(prompt + len, "\001\033[91m\002[SIG]");
    }
    else {
        sprintf(prompt + len, "\001\033[91m\002[%d]", valRes); // Commande échoué donc rouge
    }

    len += strlen(prompt+len);


    //Référence du répertoire
    char dir[PATH_MAX];

    if (getcwd(dir,sizeof(dir)) == NULL) {
        perror("cwd error");
        exit(1);
    }

    //Basculement couleur bleu et tronquer si nécessaire
    if(strlen(dir) > MAX_LENGTH-len){
        sprintf(prompt+len, "\001\033[34m\002...%s", dir + (strlen(dir) - (MAX_LENGTH-len-3)));
    }
    else {
        sprintf(prompt+len,"\001\033[34m\002%s", dir);
    }
    len += strlen(prompt + len);

    // Basculement à la couleur normale et du dollars"
    sprintf(prompt + len, "\001\033[00m\002$ ");
    len += strlen(prompt + len);

    // Affichage du prompt
    printf("%s", prompt);
    
}