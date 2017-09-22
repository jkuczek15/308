#ifndef UTIL_FUNCTIONS
#define UTIL_FUNCTIONS	

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

char *str_replace(char *orig, char *rep, char *with); 
int check_background_process_status();
char *read_line(void);

#endif
