#ifndef UTIL_FUNCTIONS
#define UTIL_FUNCTIONS	

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

char *str_replace(char *orig, char *rep, char *with); 
char *read_line(void);
char **split_line(char *line);

#endif
