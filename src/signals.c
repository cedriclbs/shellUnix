#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

void signal_handlers(void) {
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("Erreur SIGTERM");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Erreur SIGINT");
        exit(EXIT_FAILURE);
    }
}

void reinitialisation_sig () {
    struct sigaction sa_default;

        sa_default.sa_handler = SIG_DFL;
        sigemptyset(&sa_default.sa_mask);
        sa_default.sa_flags = 0;
        if (sigaction(SIGTERM, &sa_default, NULL) == -1) {
            perror("Erreur lors de la réinitialisation de SIGTERM");
            exit(EXIT_FAILURE);
        }

        if (sigaction(SIGINT, &sa_default, NULL) == -1) {
            perror("Erreur lors de la réinitialisation de SIGINT");
            exit(EXIT_FAILURE);
        }
}




 bool is_valid_signal(int sig_number) {
    switch (sig_number) {
        case SIGHUP:
        case SIGINT:
        case SIGQUIT:
        case SIGILL:
        case SIGTRAP:
        case SIGABRT:
        case SIGBUS:
        case SIGFPE:
        case SIGKILL:
        case SIGUSR1:
        case SIGSEGV:
        case SIGUSR2:
        case SIGPIPE:
        case SIGALRM:
        case SIGTERM:
        case SIGCHLD:
        case SIGCONT:
        case SIGSTOP:
        case SIGTSTP:
        case SIGTTIN:
        case SIGTTOU:
        case SIGURG:
        case SIGXCPU:
        case SIGXFSZ:
        case SIGVTALRM:
        case SIGPROF:
            return true;
        default:
            return false;
    }
}
