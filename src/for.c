#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../include/for.h"
#include "../include/builtins.h"


/**
 * Function principale pour le boucle for
 * @param args tous les mots de commande
 * @param lastExitCode lastExitCode
 * @return 0 si reussi, 1 sinon
 */
int cmd_for(char** args, int* lastExitCode)
{
    // Renvoyer 1 si un des arguments est NULL
    if (args == NULL || args[1] == NULL)
    {
        *lastExitCode = 1;
        return 1;
    }

    printf("\nArguments pas nuls\n");

    /*
     * Variables a utiliser
     */

    // Avoir la variable de boucle(fichier)
    char* file_var = args[1];
    char* file_var_dollar;
    char* input;
    char* rest;
    char** fics;
    char* commande;

    size_t mot_num_limit;

    printf("\nAvoir les variables a utiliser\n");

    /*
     * Initialiser les variables
     */

    mot_num_limit = 20;

    // Avoir la commande en un seul str(avec espaces)
    input = getWEspaces(args);
    if (input == NULL)
    {
        goto clear;
    }
    printf("\n%s\n", input);

    // Avoir les "arguments"
    rest = getEntre(input, "in", "{");;
    if (rest == NULL)
    {
        goto clear;
    }
    printf("\n%s\n", rest);

    // Avoir les arguments dans une liste de str
    fics = malloc(sizeof(char*) * mot_num_limit);
    if (fics == NULL)
    {
        goto clear;
    }
    getStrMots(rest, fics, mot_num_limit);


    // Avoir la commande a executer
    commande = getPDStr(input, "{}");
    if (commande == NULL)
    {
        goto clear;
    }
    printf("\n%s\n", commande);

    // Avoir file_var avec dollar
    file_var_dollar = malloc(strlen(file_var) + 2);
    if (file_var_dollar == NULL)
    {
        goto clear;
    }

    printf("\nInitialiser les variables a utiliser\n");

    file_var_dollar[0] = '$';
    strcpy(file_var_dollar + 1, file_var);
    file_var_dollar[strlen(file_var) + 2] = '\0';


    printf("\nInitialiser file_var_dollar\n");



    // Boucle pour executer la commande avec l'argument
    // Index de rest
    size_t i = 0;
    // Commande changÃ©
    char* new_comm;
    while (fics[i] != NULL)
    {
        new_comm = remplace(commande, file_var_dollar, fics[i]);
        // J'ai besoin de faire un fork ou pas? Je sais pas
        gereCommande(new_comm, lastExitCode);
    }

    printf("\nFait boucle pour executer la commande avec argument\n");


    // Liberer memoire
    clear:
        if (input != NULL)
        {
            free(input);
        }
        if (rest != NULL)
        {
            free(rest);
        }
        if (commande != NULL)
        {
            free(commande);
        }
        if (fics != NULL)
        {
            freeMots(fics);
        }
        if (file_var_dollar != NULL)
        {
            free(file_var_dollar);
        }
        *lastExitCode = 1;
        printf("\nJuste avant finir clear\n");
        return 1;


    free(input);
    free(rest);
    free(commande);
    freeMots(fics);
    free(file_var_dollar);
    printf("\nJuste avant finir exit normale\n");
    return 0;
}