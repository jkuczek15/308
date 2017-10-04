#include "shell_builtins.h"

/*************************************
***************************************
*   Built-in function implementations *
***************************************
***************************************/
int cd(char **args)
{
  if(args[1] != NULL){
    char* tmp = strdup(args[1]);
    args[1] = str_replace(args[1], "~", getenv("HOME"));
    
    if(strcmp(tmp, args[1]) == 0){
      args[1] = str_replace(args[1], "home", getenv("HOME"));
    }// end if the string remained unchanged

    if (chdir(args[1]) != 0) {
      perror("shell");
    }// end if changed directories successfully

    free(args[1]);
    free(tmp);
  } else {
    if (chdir(getenv("HOME")) != 0) {
      perror("shell");
    }
  }// end if the first argument is not null

  return 1;
}// end function cd

int pwd()
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL){
	printf("%s\n", cwd);
  }else{
	perror("shell");
  }// end if getcwd did not return an error
       
  return 1;	
}// end function pwd

int get(char ** args)
{  
  if (args[1] == NULL) {
     printf("'get' requires an environment variable as an argument\n");
  } else {
     
     if(getenv(args[1])){
	printf("%s\n", getenv(args[1]));
     }else{
	printf("Environment variable '%s' is not set\n", args[1]);
     }// end if environment variable is set

  }// end if argument is null
  
  return 1;
}// end function for getting environment variable

int set(char ** args)
{
  if (args[1] == NULL) {
     printf("'set' requires an environment variable and value as an argument\n");
  } else {
     
     if(args[2] == NULL){
	setenv(args[1], "", 1);
     }else{
	setenv(args[1], args[2], 1);
     }// end if environment variable is set

  }// end if argument is null
  
  return 1;
}// end function for setting environment variable

int pid()
{
  printf("PID: %d\n", getpid());
  return 1;
}// end function printing out current process ID

int ppid()
{
  printf("PPID: %d\n", getppid());
  return 1;
}// end function printing out parent process ID

int sh_exit(char **args)
{
  return 0;
}// end function exit
