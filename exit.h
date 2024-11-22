#ifndef EXIT_H
#define EXIT_H

// Vérifie si une chaîne est un entier
int is_nb(const char *str);

// Extrait le nombre après "exit"
char *extract_nb(const char *ligne);

// Traite la commande exit
int test_exit(const char *ligne);

#endif // EXIT_H
