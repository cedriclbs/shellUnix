#include <stdlib.h>
#include "../include/exit.h"

int cmd_exit(char *args) {
    int val = 0;

    // Exit avec la valeur VAL
    if (args[1] != NULL && isdigit(args[1]) {
        val = atoi(args[1]);
    }

    exit(val);
}
