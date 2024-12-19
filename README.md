# Projet SY5 : fsh - Un Shell Minimaliste

## Description du projet
`fsh` est un interpréteur de commandes (shell) interactif, conçu pour reproduire certaines fonctionnalités des shells Unix classiques. **fsh** supporte l'exécution de toutes les commandes externes, quelques commandes internes, permet la redirection des flots standard ainsi que les combinaisons par tube, adapte le prompt à la situation, et supporte les structures conditionnelles et un certain type particulier de boucles.



## Fonctionnalités

- **Commandes internes** :
    - `pwd` : Affiche la référence physique absolue du répertoire de travail courant.
    - `cd [REF | -]` : Change le répertoire de travail courant vers `REF` s'il est spécifié et s'il est un chemin valide. Sans argument, `cd` redirige vers le répertoire `$HOME`. Avec `-`, il redirige vers le répertoire précédent.
    - `exit [VAL]` : Termine le processus `fsh` avec la valeur de retour `VAL` s'il est spécifié. Sinon le shell termine avec le code de la dernière commande exécutée.
    - `ftype REF` : Affiche le type du fichier `REF` s'il est valide (`directory`, `regular file`, `symbolic link`, `named pipe`, ou `other`).

- **Exécution des commandes externes** :
    - Gestion de toutes les commandes externes, avec ou sans arguments, en tenant compte de la variable d'environnement `PATH`. 

- **Redirections** :
    - Redirection de l'entrée (`<`), sortie (`>`, `>>`) et erreur standard (`2>`, `2>>`).

- **Pipelines** :
    - Exécution des commandes simples avec `|` (en phase de développement).

- **Commandes structurées** :
    - `if TEST { CMD }` et `if TEST { CMD_1 } else { CMD_2 }`.
    - `for F in REP [-A] [-r] [-e EXT] [-t TYPE] [-p MAX] { CMD }` avec options avancées (`-A`, `-r`, `-e EXT`, `-t TYPE`, `-p MAX` en phase de développement).

- **Prompt interactif** :
    - Il retourne l'état de la dernière commande exécutée. (succès, échec ou interruption par signal).
    - Limité à 30 caractères, il inclut des codes de couleur.

- **Gestion des signaux** :
    - Ignoration du signal`SIGTERM`.
    - Interruption contrôlée avec `SIGINT`.


## Prérequis

- **Système d'exploitation** : Linux ou Windows (testé sur `lulu`).
- **Compilateur** : `gcc`.



## Installation

1. Clonez le dépôt Git :
   ```bash
   git clone https://moule.informatique.univ-paris-diderot.fr/leroyj/projet-sy5.git
   cd fsh
   ```

2. Compilez le projet avec `make` :
   ```bash
   make
   ```

3. Nettoyez les fichiers compilés avec :
   ```bash
   make clean
   ```


## Utilisation

Lancez le shell avec la commande suivante :
```
bash test.sh ou ./test.sh
```
et c'est à vous !

