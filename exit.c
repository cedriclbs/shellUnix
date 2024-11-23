#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "exit.h"

// Vérifie si une chaîne est un entier
int is_nb(const char *str) {
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Extrait le nombre après "exit"
char *extract_nb(const char *ligne) {
    const char *start = ligne;

    // Ignore les espaces, underscores, et parenthèses au début
    while (*start == ' ' || *start == '_' || *start == '(') start++;

    // Trouve la fin, en ignorant également les espaces, parenthèses, etc.
    const char *end = start;
    while (*end != '\0' && *end != ')' && *end != ' ' && *end != '\t') end++;

    // Copie du nombre
    size_t length = end - start;
    char *nb = (char *)malloc(length + 1);

    if (!nb) {
        perror("Erreur sur le malloc copie nbr");
        return NULL;
    }

    strncpy(nb, start, length);
    nb[length] = '\0';
    return nb;
}

// Traite la commande exit
int test_exit(const char *ligne) {
     // Vérifie si la commande commence par "exit"
    const char *after_exit = ligne + 4; // Déplace le pointeur après exit
    char *nb = extract_nb(after_exit); // Extrait le nombre

    if (nb == NULL || *nb == '\0') {
        // Pas dd nombre, on considère "exit" comme valide avec un code de sortie 0
        free(nb);
        return 0;
    }

    if (is_nb(nb)) {
        // Le nombre est un entier valide
        int exit_code = atoi(nb);
        free(nb);
        return exit_code;
    } else {
        free(nb);
        return -1; // Erreur 
    }

    return 0;
}
