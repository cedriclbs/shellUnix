#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include "prompt.h"

int main(){

    char *input;
    int val =0;

    //Boucle principale du shell
    while(1){

        //Affichage du prompt
        printPrompt(val);

        //Lecture de l'entrée
        input = readline("");

        //Gerer les commandes internes
        //affichage test à supprimer
        printf("Ligne saisie : %s\n", input);
        if (strcmp(input, "exit") == 0) {
            free(input); 
            break; 
        }

        //Libérer la memoire
        free(input);

        //Re affiche le prompt 
        rl_redisplay();
    }

    return 0;
}