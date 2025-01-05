#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

extern volatile sig_atomic_t sigint_received;  // Déclaration externe
extern int any_signal;
void signal_handler(int sig);

void signal_handlers();

#endif 


