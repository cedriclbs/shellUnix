#ifndef BUILTINS_H
#define BUILTINS_H

int cmd_cd(char **args);
// Permet la redirection vers un chemin, ou vers le répertoire précédent, ou vers HOME.

int cmd_exit(char **args);
// Termine le processus fsh avec comme valeur VAL

int cmd_pwd();
//Affiche la référence physique absolue du répertoire de travail courant.

int cmd_ftype (char **args);
//Affiche le type du fichier de référence REF (s'il s'agit d'une référence valide).

#endif
