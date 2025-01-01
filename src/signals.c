#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void signal_handlers(void) {
    struct sigaction sa_sigterm;

    sa_sigterm.sa_handler = SIG_IGN;
    sigemptyset(&sa_sigterm.sa_mask);
    sa_sigterm.sa_flags = 0;

    sigaction(SIGTERM, &sa_sigterm, NULL);
}