#ifndef REDIRECTIONS_H
#define REDIRECTIONS_H

char **parse_redirections(char **args, char **input_file, char **output_file, char **error_file, int *output_flags, int *error_flags, size_t *cmd_size);
int setup_redirections(const char *input_file, const char *output_file, const char *error_file, int output_flags, int error_flags);

#endif 