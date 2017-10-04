/*************************************
**************************************
  Includes and Definitions
**************************************
**************************************/
#include "utils.h"
#include "bank_functions.h"
#include "appserver.h"

/*************************************
**************************************
  	   Main Server Loop
**************************************
**************************************/
void bank_loop(int workers, int accounts) 
{
  char *line;
  char **args;
  int accountID, num_args;

  while(1)
  {
     printf("Bank> ");
     line = read_line();
     num_args = num_arguments(line);
     args = split_line(line);

     if (strcicmp(line, "END") == 0){
	// exit the program
	printf("Exiting...\n");
	free(line);
	break;
     }else if(strcicmp(line, "CHECK") == 0){
	// read from bank account
 	if(num_args != 2){
	   printf("Usage: check <accountID>\n");
	   continue;
	}// end if wrong number of arguments to 'check'	
	
	// convert the account ID argument to integer
 	accountID = atoi(args[1]);
	int balance = getBalance(accountID, accounts);

	if(balance >= 0){
	  printf("Balance: %d\n", balance);
	}// end if we didnt have an error grabbing the balance
     }// end if user issued the END command

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
	
	initialize_accounts(num_accounts);
	bank_loop(num_worker_threads, num_accounts);		
  }else {
     printf("%s", usage);
     return 0;
  }// end if argc != 3
  
  return 0;
}// end main function
