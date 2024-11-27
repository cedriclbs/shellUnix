#include <stdio.h>
#include <unistd.h>
#include "../include/pwd.h"

int cmd_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("fsh: erreur\n");
        return 1;
    }
}
