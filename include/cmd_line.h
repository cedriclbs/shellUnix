#ifndef CMD_LINE_H
#define CMD_LINE_H

int cmd_line(char **args);

char **copy_cmd(char *cur_cmd[], int cur_cmd_size);

char ***split_cmd(char *args[], int *nb, char *delimiter);

#endif
