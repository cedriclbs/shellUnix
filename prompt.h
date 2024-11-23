#ifndef PROMPT_H
#define PROMPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define MAX_DISPLAY_LENGTH 30

char* getPrompt(int valRes);

#endif