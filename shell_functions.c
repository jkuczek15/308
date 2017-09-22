#include "shell_functions.h"

/*************************************
**************************************
  Shell function implementations
**************************************
**************************************/
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
      exit(EXIT_FAILURE);
    }// end if error while executing command
    exit(EXIT_SUCCESS);
  } else if (pid < 0) {
    // error forking
    perror("shell");
  } else {
    // parent process
    printf("[%d]\n", pid);
    if(!background) {
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
        printf("[%d] Exit %d\n", wpid, status);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }// end if parent is not running a background process  
    
  }// end if pid == 0

  return 1;
}// end function launch

/*
  Execute either a built-in shell command, or spawn child process for other commands
*/
int execute(char **args)
{
  int i;
   
  check_background_process_status();

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
  Get number of built-in shell commands
*/
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}// end function num_builtins

