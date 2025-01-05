#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "builtins.h"
#include "signal.h"
#include "signals.h"


/**
 * Alloue et copie les éléments d'une commande dans un nouveau tableau de chaînes de caractères.
 * Chaque commande est terminée par un pointeur NULL pour l'utiliser avec les fonctions exec.
 *
 * @param cur_cmd Tableau des chaînes à copier.
 * @param cur_cmd_size Taille du tableau cur_cmd.
 * @return Pointeur vers le nouveau tableau de chaînes.
 */
char **copy_cmd(char *cur_cmd[], int cur_cmd_size) {
    char **cmd = malloc((cur_cmd_size + 1) * sizeof(char *));
    
    for (int j = 0; j < cur_cmd_size; j++) {
        cmd[j] = cur_cmd[j];  
    }

    cmd[cur_cmd_size] = NULL; 

    return cmd;
}

/**
 * Divise une liste d'arguments en sous-commandes basées sur le caractère ';',
 * en ignorant les ';' qui se trouvent entre des paires d'accolades '{' et '}'.
 *
 * @param args Tableau de chaînes représentant les arguments entiers.
 * @param nb Pointeur pour stocker le nombre de commandes divisées.
 * @return Tableau de tableaux de chaînes, chaque tableau représentant une commande.
 */
char ***split_cmd(char *args[], int *nb, char *delimiter) {
    char ***ret_tab = malloc(100 * sizeof(char **));  
    int count = 0;
    int i = 0;
    int in_braces = 0;  

    char *cur_cmd[100];  
    int cur_cmd_size = 0;

    while (args[i] != NULL) {
        if (strcmp(args[i], "{") == 0) {
            in_braces++;  // Entrée dans une paire d'accolades
        } else if (strcmp(args[i], "}") == 0) {
            in_braces--;  // Sortie de la paire d'accolades
        }

        // Si nous ne sommes pas dans des accolades, gérer les points-virgules
        if (in_braces == 0 && strcmp(args[i], delimiter) == 0) {  
            if (cur_cmd_size > 0) {
                ret_tab[count] = copy_cmd(cur_cmd, cur_cmd_size);  // Copie la commande dans le tableau final
                count++;
                cur_cmd_size = 0; 
            } else {
                return NULL;  
            }
        } else {
            cur_cmd[cur_cmd_size] = args[i];  
            cur_cmd_size++; 
        }
        i++;
    }

    if(strcmp(delimiter, "|") == 0 && cur_cmd_size == 0 ){
        perror("pipelines: Erreur de syntaxe");
        return NULL; /* à vérifier */
    }

    // Copierla dernière commande si elle existe
    if (cur_cmd_size > 0) {
        ret_tab[count] = copy_cmd(cur_cmd, cur_cmd_size);
        count++;
    }

    *nb = count;  
    return ret_tab;
}

/**
 * Exécute une commande en créant un processus enfant et en invoquant la fonction appropriée.
 * Utilise fork pour créer un processus enfant et waitpid pour attendre sa terminaison.
 *
 * @param cmd Commande à exécuter.
 * @param argc Nombre d'arguments dans cmd.
 * @param val Valeur à passer à la fonction builtin si nécessaire.
 * @return Code de sortie de la commande exécutée.
 */
int execute_cmd(char **cmd, int argc, int val,int *isSigint) {
    pid_t pid = fork(); 
    int ret = 0;
    //reinitialisation_sig();

    switch (pid) {
        case -1:
            perror("Erreur fork");
            exit(1);

        case 0:  // Code du processus enfant
            ret = execute_builtin(cmd, argc, val);
            exit(ret); 
             

        default:  // Code du processus parent
            int status;
            waitpid(pid, &status, 0);  
            if (WIFEXITED(status)) {
                ret = WEXITSTATUS(status);  
            }
            return ret;
    }
}

/**
 * Traite une ligne de commande complète, en divisant la commande en sous-commandes et en les exécutant.
 * Gère la séparation des commandes, leur exécution séquentielle et la libération des ressources allouées.
 *
 * @param args Tableau de chaînes contenant la ligne de commande complète à exécuter.
 * @return Code de sortie de la dernière commande exécutée ou 1 en cas d'erreur.
 */
int cmd_line(char **args) {
    signal_handlers();
    int nb_cmd = 0;
    char ***cmds = split_cmd(args, &nb_cmd, ";");
    int isSigint =0;  

    if (cmds == NULL) {
        return 1;
    }

    int ret = 0;
    for (int i = 0; i < nb_cmd; i++) {
        int cmd_size = 0;
        // Calcule la taille de la commande actuelle (le nombre d'arguments)
        while (cmds[i][cmd_size] != NULL) {
            cmd_size++;
        }
        ret = execute_cmd(cmds[i], cmd_size, ret,&isSigint);
        if(sigint_received) break;
    }
     printf("le processus PARENT %d\n", getpid());
    for (int i = 0; i < nb_cmd; i++) {
        free(cmds[i]);  
    }
    free(cmds);  

    return ret;
}

