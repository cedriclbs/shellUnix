#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigterm_received = 0;
int any_signal = 0;

void updatesig(int sig) {
    switch (sig) {
        case SIGINT:
            sigint_received = 1;
            any_signal = 1;
            break;
        case SIGTERM:
            sigterm_received = 1;
            any_signal = 1;
            break;
        default:
            break;
    }
}

void signal_handler(int sig) {
    updatesig(sig);
}

void resetSigs() {
    sigint_received = 0;
    sigterm_received = 0;
    any_signal = 0;
}

void unblockSignals() {
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGTERM);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_UNBLOCK, &new_mask, NULL);
}

// Installation des gestionnaires
void signal_handlers() {
    struct sigaction sa;
    sigset_t mask;

    // Initialisation complète de la structure sigaction
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART; 

    // Initialisation du masque de signaux
    sigemptyset(&sa.sa_mask);
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