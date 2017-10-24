/*************************************
**************************************
  Includes and Definitions
**************************************
**************************************/
#include "utils.h"
#include "bank_functions.h"
#include "queue.h"
#include "appserver.h"

// main linked list data structure for holding requests
// to be serviced by the worker threads
struct Queue* requests;

// number of total bank accounts
int num_accounts;

// lock for our queue
pthread_mutex_t queue_lock;

// store a lock for each bank account
pthread_mutex_t *locks;

/*************************************
**************************************
  	   Main I/O Loop
**************************************
**************************************/
void bank_loop(int accounts) 
{
  char *line;
  char **args;
  int accountID, num_args, i, id, value, balance;
  
  // create requests to store in the queue
  struct Request *request;

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
	balance = getBalance(accountID, accounts);

	if(balance >= 0){
	  printf("Balance: %d\n", balance);
	}// end if we didnt have an error grabbing the balance
     }else if(strcicmp(line, "TRANS") == 0){
       // make a transaction to multiple accounts
       if(num_args % 2 == 0){
	 printf("Usage: trans <account1> <amount1> <account2> <amount2> ... <accountk> <amountk>\n");
         continue;
       }// end if wrong number of arguments to 'check'
       
       for(i = 1; i < num_args-1; i++){
	 id = atoi(args[i]);
 	 value = atoi(args[i+1]);
         request = createRequest(id, value, 1);
	 enqueue(requests, request);
       }// end for loop over all arguments
	
     }// end if user issued the END command
     
     free(line);
     free(args);
  }// end main server while loop taking requests
  
  free(requests);
}// end function bank_loop()

/*************************************
**************************************
  	Worker Thread Loop
**************************************
**************************************/
void *worker_loop(void *args)
{
  struct Request *request;
 
  while(1)
  {
    // while loop servicing requests from main I/O thread
    // first lock the queue before we do any operations on it 
    pthread_mutex_lock(&queue_lock);

    if(!isEmpty(requests)){
      // we have a request we can service
      request = dequeue(requests);

      // now we can unlock the queue
      pthread_mutex_unlock(&queue_lock);
      
      if(request->accountID != 0){
	 // lock the mutex, make the transaction, unlock the mutex
	 pthread_mutex_lock(&locks[request->accountID-1]);
	 //printf("Account ID: %d, Value: %d\n", request->accountID, request->value);
         transaction(request->accountID, request->value, num_accounts);
         pthread_mutex_unlock(&locks[request->accountID-1]);
      }// end if we have a nonzero account ID

     }else{
       // queue is empty, just unlock it for another thread
       pthread_mutex_unlock(&queue_lock);
     }// end if the queue is not empty, service a request   

  }// end while loop for worker thread servicing requests
  
  if(request){
    free(request);
  }// end if there was recently a request, free it
}// end function worker_loop()

/*************************************
**************************************
  	Main function
**************************************
**************************************/
int main(int argc, char** argv) 
{
  char *usage = "Usage: ./appserver <# of worker threads> <# of accounts> <output file>\n";
  int num_worker_threads, i;
  pthread_t thread;
	
  if(argc == 4) {   
	num_worker_threads = atoi(argv[1]);
	num_accounts = atoi(argv[2]);
 	
	if(num_worker_threads == 0 || num_accounts == 0){
		printf("Error parsing number of worker threads/number of accounts. Enter nonzero integers for both values.\n");
		return 0;
	}// end if num_worker_threads == 0 or num_accounts == 0 	
	
	// initialize the accounts to have balance of 0
	initialize_accounts(num_accounts);
  	
	// allocate space for storing our mutexes	
	locks = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * num_accounts);

	// initialize a lock for the queue 	
	pthread_mutex_init(&queue_lock, NULL);

	for(i = 0; i < num_accounts; i++){
	  // initialize a new mutex for each account
	  pthread_mutex_t mutex;	 
	  pthread_mutex_init(&mutex, NULL);
	  locks[i] = mutex;
	}// end for loop over number of accounts

	// create a new queue for handling requests
        requests = createQueue(1000);

	for(i = 0; i < num_worker_threads; i++){
	  // initalize the worker threads to service requests
	  pthread_create(&thread, NULL, worker_loop, NULL); 
	}// end for loop creating worker threads
	
	// run our main I/O loop servicing requests
	bank_loop(num_accounts);			
  }else {
     printf("%s", usage);
     return 0;
  }// end if argc != 3
  
  return 0;
}// end main function


