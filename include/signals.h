#ifndef SIGNALS_H
#define SIGNALS_H
#include <stdbool.h>
#include <signal.h>

void signal_handlers(void);

void reinitialisation_sig(void);

bool is_valid_signal(int sig_number);

#endif // SIGNALS_H
