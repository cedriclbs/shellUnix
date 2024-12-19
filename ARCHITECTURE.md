## Structure du projet


### Fichiers source (`/src`)
Le dossier `/src` contient tous les fichiers source chacun responsable d'une partie spécifique du fonctionnement de `fsh`.

- **fsh.c** : Contient la boucle principale du shell.
- **pwd.c** : Implémente la commande interne `pwd`.
- **cd.c** : Implémente la commande interne `cd`.
- **exit.c** : Implémente la commande interne `exit`.
- **ftype.c** : Implémente la commande interne `ftype`.
- **redirections.c** : Gère les redirections d'entrée/sortie/erreur standard.
- **for.c** : Implémente les boucles `for` et leurs options (`-A`, `-r`, `-e EXT`, `-t TYPE`, `-p MAX`)
- **ifelse.c** : Implémente les structures conditionnelles (`if`, `if...else`).
- **execute.c** : Gère l'exécution des commandes avec des redirections éventuelles en utilisant les appels système fork et execvp.
- **prompt.c** : Gère l'affichage du prompt personnalisé pour le shell, incluant le code de retour, le répertoire actuel et un symbole de prompt.
- **cmd_line.c** : Alloue et copie les éléments d'une commande dans un nouveau tableau de chaînes de caractères.
- **builtins.c** : Vérifie la présence d'une sous-chaîne dans les arguments de la commande.


### Fichiers include (`/include`)

Le dossier `/include` contient tous les fichiers d'en-tête nécessaires au projet, permettant une organisation cohérente.

- **pwd.h** : Déclare les fonctions associées à la commande interne `pwd`, affichant le répertoire de travail courant.
- **cd.h** : Déclare les fonctions associées à la commande interne `cd`, permettant de changer le répertoire de travail.
- **exit.h** : Déclare les fonctions associées à la commande interne  `exit`, permettant de quitter le shell proprement.
- **ftype.h** : Déclare les fonctions associées à la commande interne `ftype`, utilisée pour afficher le type d'un fichier.
- **redirections.h** : Définit les fonctions gérant les redirections d'entrée, sortie, et erreur standard.
- **for.h** : Définit les fonctions nécessaires pour l'implémentation des boucles `for` avec leurs options.
- **ifelse.h** : Contient les déclarations des fonctions permettant de gérer les structures conditionnelles.
- **execute.h** : Définit les fonctions utilisées pour exécuter les commandes externes.
- **prompt.h** : Déclare les fonctions permettant de générer et d’afficher le prompt dynamique du shell.
- **cmd_line.h** : Déclare les structures et fonctions utilisées pour manipuler la ligne de commande.
- **builtins.h** : Contient les déclarations des fonctions utilitaires pour vérifier et gérer les commandes internes.


### Fichiers build (`/build`)

Le dossier `/build` crée suite à la compilation, regroupe tous les fichiers objets (.o) contenant le code compilé de chacun des fichiers sources.
