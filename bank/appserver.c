/*************************************
**************************************
  Includes and Definitions
**************************************
**************************************/
#include "utils.h"
#include "appserver.h"

void loop() 
{
  char *line;
  char **args;
  
  while(1)
  {
     printf("Bank> ");
     line = read_line();
     
     if (strcmp(line, "END\n") == 0){
	printf("Exiting...\n");
	free(line);
	break;
     }// end if user issued the END command

     args = split_line(line);
     printf("%s\n", line);
     
     free(line);
     free(args);
  }// end main server while loop taking requests

 }// end function loop


/*************************************
**************************************
  	Main function
**************************************
**************************************/
int main(int argc, char** argv) 
{
  char *usage = "Usage: ./appserver <# of worker threads> <# of accounts> <output file>\n";
  int num_worker_threads, num_accounts;

  if(argc == 4) {
    
	num_worker_threads = atoi(argv[1]);
	num_accounts = atoi(argv[2]);
 	if(num_worker_threads == 0 || num_accounts == 0){
		printf("Error parsing number of worker threads/number of accounts. Enter nonzero integers for both values.\n");
		return 0;
	}// end if num_worker_threads == 0 or num_accounts == 0 	
	loop();		
  }else {
     printf("%s", usage);
     return 0;
  }// end if argc != 3
  
  return 0;
}// end main function
