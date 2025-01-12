#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigterm_received = 0;
int any_signal = 0;

/**
 * @brief Met à jour les indicateurs pour les signaux reçus.
 *
 * Cette fonction est appelée lorsqu'un signal est capturé. Elle met à jour les variables
 * globales pour indiquer quel signal a été reçu.
 *
 * @param sig Le signal capturé (exemple : `SIGINT`, `SIGTERM`).
 */
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

/**
 * @brief Gestionnaire de signaux générique.
 *
 * Capture les signaux spécifiés et appelle la fonction `updatesig` pour mettre à jour
 * les indicateurs appropriés.
 *
 * @param sig Le signal capturé.
 */
void signal_handler(int sig) {
    updatesig(sig);
}

/**
 * @brief Réinitialise les indicateurs de signaux.
 *
 * Cette fonction remet à zéro les variables globales indiquant si des signaux ont été reçus.
 * Elle est utile pour préparer le programme à capturer de nouveaux signaux.
 */
void resetSigs() {
    sigint_received = 0;
    sigterm_received = 0;
    any_signal = 0;
}

/**
 * @brief Débloque les signaux SIGINT et SIGTERM.
 *
 * Cette fonction utilise `sigprocmask` pour supprimer le masquage de SIGINT et SIGTERM,
 * permettant au programme de capturer ces signaux.
 */
void unblockSignals() {
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGTERM);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_UNBLOCK, &new_mask, NULL);
}

/**
 * @brief Configure les gestionnaires de signaux pour le programme.
 *
 * Cette fonction configure les gestionnaires de signaux pour `SIGINT` et masque globalement `SIGTERM`.
 * Elle utilise `sigaction` pour définir un comportement sûr et reproductible, incluant :
 * - Redémarrer les appels système interrompus (`SA_RESTART`).
 * - Bloquer SIGINT pendant son traitement.
 *
 * ### Comportement :
 * - SIGINT (`Ctrl+C`) est capturé et traité via `signal_handler`.
 * - SIGTERM est globalement bloqué pour éviter des interruptions non gérées.
 *
 * ### Gestion des erreurs :
 * Si la configuration des gestionnaires ou du masque échoue, le programme affiche un message
 * d'erreur et termine avec un code de sortie 1.
 */
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