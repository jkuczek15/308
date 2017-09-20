/*************************************
**************************************
  Includes and Definitions
**************************************
**************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

/*************************************
**************************************
  Helpers and Function Declarations
**************************************
**************************************/
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
void loop ();

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

/*
  Number of arguments for commands inside the shell
*/
int num_args;

/*
 User prompt for the shell
*/
char *prompt;

/*
  Get number of built-in shell commands
*/
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}// end function num_builtins

// You must free the result if result is non-NULL.
char *str_replace(char *orig, char *rep, char *with) 
{
    char *result;  // the return string
    char *ins;     // the next insert point
    char *tmp;     // varies
    int len_rep;   // length of rep (the string to remove)
    int len_with;  // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;     // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    // tmp points to the end of the result string
    // ins points to the next occurrence of rep in orig
    // orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}// end function str_replace()

/*************************************
**************************************
  Built-in function implementations
**************************************
**************************************/
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

/*************************************
**************************************
  Shell function implementations
**************************************
**************************************/
/*
  Read line from standard input
*/
char *read_line(void)
{
  char *line = NULL;
  size_t bufsize = 0;
  getline(&line, &bufsize, stdin);
  return line;
}// end function read_line

/*
  Split the line from standard input (get arguments)
*/
char **split_line(char *line)
{
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "shell: Allocation error\n");
    exit(EXIT_FAILURE);
  }// end if tokens not allocated 

  token = strtok(line, TOK_DELIM);
  
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      
      if (!tokens) {
        fprintf(stderr, "shell: Allocation error\n");
        exit(EXIT_FAILURE);
      }// end if !tokens

    }// end if position >= bufsize

    token = strtok(NULL, TOK_DELIM);
  }// end while token is not null

  tokens[position] = NULL;
  num_args = position;
  return tokens;
}// end function split_line

/*
  Execute a command by spawning a new child process
*/
int launch(char **args)
{
  pid_t pid, wpid;
  int status;
  fflush(stdout); 
  int background = 0;

  if(args[num_args-1] && strcmp(args[num_args-1], "&") == 0){
     background = 1;
     args[num_args-1] = NULL;
     free(args[num_args-1]);
  }// end if last character is '&'

  pid = fork();
  if (pid == 0) {
    // child process 
    if (execvp(args[0], args) == -1) {
      char error_message[1024];
      snprintf(error_message, sizeof(error_message), "Cannot execute %s", args[0]);
      perror(error_message);
    }// end if error while executing command
    exit(EXIT_SUCCESS);
  } else if (pid < 0) {
    // error forking
    perror("shell");
  } else {
    // parent process
    printf("[%d]\n", pid);
    if(!background){
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
        printf("[%d] Exit %d\n", wpid, status);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }// end if parent is not running a background process  
    
    // check for any exited background processes  
    do {
      wpid = waitpid(-1, &status, WNOHANG);
      if(wpid != 0 && wpid != -1){
        printf("[%d] Exit %d\n", wpid, status);
      }// end if we have valid exited process ID
    } while(wpid != 0 && wpid != -1);

  }// end if pid == 0

  return 1;
}// end function launch

/*
  Execute either a built-in shell command, or spawn child process for other commands
*/
int execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1;
  }// end if command was empty

  for (i = 0; i < num_builtins(); i++) {
    
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }// end if this is a built in command

  }// end for loop over number of built in commands

  return launch(args);
}// end function execute

/*
  Main shell loop
*/
void loop() 
{
  char *line;
  char **args;
  int status;

  do {
    printf("%s> ", prompt);
    line = read_line();
    args = split_line(line);
    status = execute(args);
    free(line);
    free(args);
  } while (status);

}// end function loop

/*************************************
**************************************
  	Main function
**************************************
**************************************/
int main(int argc, char** argv) 
{
  prompt = "308sh";
  char *usage = "Usage: program -p <prompt>\n";

  if(argc == 3) {
    
    if(strcmp(argv[1], "-p")  == 0){
      prompt = argv[2];
    }else{
      printf("%s", usage);
      return 0;
    }// end if they passed the -p argument

  }else if(argc != 1){
     printf("%s", usage);
     return 0;
  }// end if argc != 3
  
  // loop the shell receiving input from the user
  loop();

  return 0;
}// end main function
