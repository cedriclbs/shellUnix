#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// Variables globales pour signaler l'interruption
volatile sig_atomic_t sigint_received = 0;
int any_signal=0;

void signal_handler(int sig) {
    (void)sig; // Supprime l'avertissement pour la variable non utilisée
    printf("Signal %d reçu dans le processus %d\n", sig, getpid());
    sigint_received = 1; // Signal reçu
}

void handle_sigterm(int sig) {
}

// Installation des gestionnaires
void signal_handlers() {
    struct sigaction sa;
    sigset_t mask;

    // Initialisation complète de la structure sigaction
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    
    // Configuration des flags
    sa.sa_flags = SA_RESTART; // Redémarrer les appels système interrompus
    
    // Initialisation du masque de signaux
    sigemptyset(&sa.sa_mask); // Vider le masque pendant le gestionnaire
    sigaddset(&sa.sa_mask, SIGINT); // Bloquer SIGINT pendant son traitement
    
    // Installation du gestionnaire pour SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("SIGINT");
        exit(1);
    }

    // Configuration du masque global des signaux
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    
    // Bloquer SIGTERM au niveau du processus
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("Erreur masque de signaux");
        exit(1);
    }
}
