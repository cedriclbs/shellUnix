# Projet SY5 : fsh - Un Shell Minimaliste

## Description du projet
Le projet **fsh** est un interpréteur de commandes (shell) interactif, conçu pour reproduire certaines fonctionnalités des shells Unix classiques. **fsh** supporte l'exécution de toutes les commandes externes, quelques commandes internes, permet la redirection des flots standard ainsi que les combinaisons par tube, adapte le prompt à la situation, et supporte les structures conditionnelles et un certain type particulier de boucles.

### Commandes Internes
fsh inclut plusieurs commandes internes exécutables :

- **cd [REF | -]** : Change le répertoire de travail courant vers `REF` s'il est spécifié et s'il est un chemin valide. Sans argument, `cd` redirige vers le répertoire `$HOME`. Avec `-`, il redirige vers le répertoire précédent.
- **pwd** : Affiche le référence physique absolue du répertoire de travail courant.
- **exit [VAL]** : Termine le shell avec le code de retour `VAL` s'il est spécifié. Sinon le shell termine avec le code de la dernière commande exécutée.
- **ftype REF** : Affiche le type du fichier référencé par `REF` s'il est valide, comme `directory`, `regular file`, `symbolic link`, `named pipe`, ou `other`.


### Lancer et Utiliser fsh
1. Compilez le projet avec `make` à partir de la racine du dépôt.
2. Lancez le shell avec la commande suivante : 