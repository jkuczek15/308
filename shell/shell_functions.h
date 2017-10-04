#ifndef SHELL_FUNCTIONS
#define SHELL_FUNCTIONS

#include "shell_builtins.h"
#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

#ifndef SHELL
/*
  List of built-in commands
*/
char *builtin_str[] = {
  "cd",
  "pwd",
  "get",
  "set",
  "pid",
  "ppid", 
  "exit"
};

/*
  List of corresponding functions for built-in shell commands
  Note: some functions are prefixed with "sh" because otherwise they would be C keywords
*/
int (*builtin_func[]) (char **) = {
  &cd,
  &pwd,
  &get,
  &set,
  &pid,
  &ppid,
  &sh_exit
};
#endif

/*
  Number of arguments for commands inside the shell
*/
int num_args;
int launch(char **args);
int execute(char **args);
char **split_line(char *line);
int num_builtins();

#endif
