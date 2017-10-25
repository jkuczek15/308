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

// number of worker threads
int num_worker_threads;

// request ID of the current request being made
int requestID = 1;

// use this to tell worker threads to finish when main
// thread is done
int done = 0;

// lock for our queue
pthread_mutex_t queue_lock;

// store a lock for each bank account
pthread_mutex_t *locks;

// store our threads in a list for easy access
pthread_t *threads;

// array of integers representing the workers tht are done
int *threads_done;

// the output file pointer we write too
FILE *output;

// timestamp variable
struct timeval start, end;

/*************************************
**************************************
  	  Bank I/O Loop
**************************************
**************************************/
void bank_loop() 
{
  char *line;
  char **args;
  int num_args, i, id, value;
  
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
	done = 1;
	break;
     }else if(strcicmp(line, "CHECK") == 0){
	// read from bank account
 	if(num_args != 2){
	   printf("Usage: check <accountID>\n");
	   continue;
	}// end if wrong number of arguments to 'check'	

	printf("ID %d\n", requestID);

	// convert the account ID argument to integer
 	id = atoi(args[1]);
	request = createRequest(requestID, id, 0, 0);
	enqueue(requests, request);
	requestID++;
     }else if(strcicmp(line, "TRANS") == 0){
       // make a transaction to multiple accounts
       if(num_args % 2 == 0){
	 printf("Usage: trans <account1> <amount1> <account2> <amount2> ... <accountk> <amountk>\n");
         continue;
       }// end if wrong number of arguments to 'check'

       printf("ID %d\n", requestID);
 
       for(i = 1; i < num_args-1; i += 2){
	 id = atoi(args[i]);
 	 value = atoi(args[i+1]);
         request = createRequest(requestID, id, value, 1);
	 enqueue(requests, request);
       }// end for loop over all arguments

       requestID++;	
     }// end if user issued the END command
     
     free(line);
     free(args);
  }// end main server while loop taking requests

  // wait for all worker threads to finish, then free our buffer
  wait_for_workers(threads_done, num_worker_threads);
  free(requests);
}// end function bank_loop()

/*************************************
**************************************
  	Worker Thread Loop
**************************************
**************************************/
void *worker_loop(void *id)
{
  struct Request *request;
  int balance, error;
  int thread_id = *((int *) id);

  while(1)
  {
    // while loop servicing requests from main I/O thread
    // first lock the queue before we do any operations on it 
    pthread_mutex_lock(&queue_lock);
    
    if(!isEmpty(requests)){
      // we have a request we can service
      request = dequeue(requests);
      // get the current time
      gettimeofday(&start, NULL);

      // now we can unlock the queue
      pthread_mutex_unlock(&queue_lock);
      
      if(request->write){
	// we have a write request
	// lock the mutex, make the transaction, unlock the mutex
	pthread_mutex_lock(&locks[request->accountID-1]);
        error = transaction(request->accountID, request->value, num_accounts);
        pthread_mutex_unlock(&locks[request->accountID-1]);
	
	gettimeofday(&end, NULL);
        if(error == -1){
	  fprintf(output, "%d INVALID ACCOUNT ID TIME %d.%06d %d.%06d\n", request->requestID, (int) start.tv_sec, (int) start.tv_usec, (int) end.tv_sec, (int) end.tv_usec);
	}else if(error == -2){
	  fprintf(output, "%d ISF %d TIME %d.%06d %d.%06d\n", request->requestID, request->accountID, (int) start.tv_sec, (int) start.tv_usec, (int) end.tv_sec, (int) end.tv_usec);
	}else{
	  fprintf(output, "%d OK TIME %d.%06d %d.%06d\n", request->requestID, (int) start.tv_sec, (int) start.tv_usec, (int) end.tv_sec, (int) end.tv_usec);
	}// end if there was an error in the transaction

      }else{
	// we have a read request
	balance = getBalance(request->accountID, num_accounts);
	
	gettimeofday(&end, NULL);
	if(balance >= 0){
          fprintf(output, "%d BAL %d TIME %d.%06d %d.%06d\n", request->requestID, balance, (int) start.tv_sec, (int) start.tv_usec, (int) end.tv_sec, (int) end.tv_usec);
	}else{
	  fprintf(output, "%d INVALID ACCOUNT ID TIME %d.%06d %d.%06d\n", request->requestID, (int) start.tv_sec, (int) start.tv_usec, (int) end.tv_sec, (int) end.tv_usec);
	}// end if we didnt have an error grabbing the balance

      }// end if this is a write request
            
     }else if(done){
	// queue is empty, and user asked to exit
	pthread_mutex_unlock(&queue_lock);
	break;
     }else{
       // queue is empty, just unlock it for another thread
       pthread_mutex_unlock(&queue_lock);
     }// end if the queue is not empty, service a request   

  }// end while loop for worker thread servicing requests
  
  threads_done[thread_id] = 1;
  return 0;
}// end function worker_loop()

/*************************************
**************************************
  	Main function
**************************************
**************************************/
int main(int argc, char** argv) 
{
  char *usage = "Usage: ./appserver <# of worker threads> <# of accounts> <output file>\n";
  char *filename;
  int i;
  
  if(argc == 4) {   
    num_worker_threads = atoi(argv[1]);
    num_accounts = atoi(argv[2]);
    filename = argv[3];
    output = fopen(filename, "w");
    
    if(output == NULL){
      printf("Error opening file!\n");
      exit(1);
    }// end if error opening the output file
    
    // allocate space for arguments
    int *args = malloc(sizeof(*args) * num_worker_threads);

    if(num_worker_threads == 0 || num_accounts == 0){
      printf("Error parsing number of worker threads/number of accounts. Enter nonzero integers for both values.\n");
      return 0;
    }// end if num_worker_threads == 0 or num_accounts == 0 	
	
    // initialize the accounts to have balance of 0
    initialize_accounts(num_accounts);
  	
    // allocate space for storing our mutexes	
    locks = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * num_accounts);
	
    // allocate space for storing thread information
    threads = (pthread_t *) malloc(sizeof(pthread_t) * num_worker_threads);

    // initialize a lock for the queue 	
    pthread_mutex_init(&queue_lock, NULL);

    // initialize an integer array for keeping track of threads that are done
    threads_done = (int *) malloc(sizeof(int) * num_worker_threads);

    for(i = 0; i < num_accounts; i++){
      // initialize a new mutex lock for each account
      pthread_mutex_t mutex;	 
      pthread_mutex_init(&mutex, NULL);
      locks[i] = mutex;
    }// end for loop over number of accounts

    // create a new queue for handling requests
    requests = createQueue(1000);

    for(i = 0; i < num_worker_threads; i++){
      // initalize the worker threads to service requests
      args[i] = i;
      threads_done[i] = 0;
      pthread_create(&threads[i], NULL, worker_loop, &args[i]); 
    }// end for loop creating worker threads
	
    // run our main I/O loop servicing requests
    bank_loop();

    for(i = 0; i < num_worker_threads; i++){
      // join the main thread to every other thread, so main finishes last
      pthread_join(threads[i], NULL); 
    }// end for loop creating worker threads

    fclose(output);
  }else {
     printf("%s", usage);
     return 0;
  }// end if argc != 3
  
  if(locks){
    free(locks);
  }// end if we need to free lock memory

  if(threads){
    free(threads);
  }// end if we need to free memory for storing thread info
  
  return 0;
}// end main function


