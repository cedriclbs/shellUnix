#include <stdio.h>
#include <sys/stat.h>
#include "../include/ftype.h"

int cmd_ftype(char **args) {
    struct stat st;

    if (args[1] == NULL) {
        fprintf(stderr, "ftype : argument manquant\n");
        return 1;
    }

    if (lstat(args[1], &st) < 0) {
        perror("ftype : erreur\n");
        return 1;
    }

    if (S_ISDIR(st.st_mode)) {
        printf("directory\n");
    } else if (S_ISREG(st.st_mode)) {
        printf("regular file\n");
    } else if (S_ISLNK(st.st_mode)) {
        printf("symbolic link\n");
    } else if (S_ISFIFO(st.st_mode)) {
        printf("named pipe\n");
    } else {
        printf("other\n");
    }

    return 0;
}
