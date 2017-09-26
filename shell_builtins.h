#ifndef SHELL_BUILT_IN_FUNCTIONS
#define SHELL_BUILT_IN_FUNCTIONS
#include "utils.h"

/*
  Declarations for built-in shell commands:
*/
int cd(char **args);
int pwd();
int get(char **args);
int set(char **args);
int pid();
int ppid();
int sh_exit(char **args);

#endif
