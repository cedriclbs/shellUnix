src
 ┣ builtins.c
 ┣ cd.c
 ┣ cmd_line.c
 ┣ execute.c
 ┣ exit.c
 ┣ for.c
 ┣ fsh.c
 ┣ ftype.c
 ┣ ifelse.c
 ┣ pipelines.c
 ┣ prompt.c
 ┣ pwd.c
 ┣ redirections.c
 ┗ signals.c


builtins.c : Gestion de l'ensemble des exécutions de commandes

cd.c : Implémentation de la commande interne "cd"

cmd_line.c : Gestion des commmandes structurées de type "CMD_1 ; CMD_2 ; ... ; CMD_N"

execute.c : Gestion des commandes externes

exit.c : Implémentation de la commande interne "exit"

for.c : Implémentation de la boucle for

fsh.c : Programme fsh, boucle principale du shell

ftype.c : Implémentation de la commande interne "ftype"

ifelse.c : Gestions des commandes structurées de type "if TEST { CMD_1 } else { CMD_2 }" et "if TEST { CMD }"

pipelines.c : Gestions des pipelines

prompt.c : Gestion de la ligne de commande

pwd.c : Implémentation de la commande interne "pwd"

redirections.c : Gestion des redirections (vérifié s'il y en a, mettre en place les descripteurs associés...)

signals.c : Gestion des signaux