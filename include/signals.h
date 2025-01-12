#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

extern volatile sig_atomic_t sigint_received;  
extern volatile sig_atomic_t sigterm_received;
extern int any_signal;

void updatesig(int sig);
void signal_handler(int sig);
void resetSigs();
void unblockSignals();
void signal_handlers();

#endif 


