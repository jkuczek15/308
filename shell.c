/*************************************
**************************************
  Includes and Definitions
**************************************
**************************************/
#include "shell_builtins.h"
#include "shell.h"
#include "shell_functions.h"

/*************************************
**************************************
           Main Shell Loop
**************************************
**************************************/
void loop(char *prompt) 
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
  char *prompt = "308sh";
  char *usage = "Usage: ./shell -p <prompt>\n";

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
  loop(prompt);

  return 0;
}// end main function
