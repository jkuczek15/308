#include "utils.h"

/*
 * Compare two strings, case insensitive
*/
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }// end for loop comparing each character of the string
}// end case insensitive string compare

/*
* Get the number of arguments for a particular string, creates a copy of the string
*/
int num_arguments(char *linep)
{

  char *line = malloc (1 + strlen(linep));
  
  if (line){
    strcpy (line, linep);
  }else{
    fprintf (stderr, "malloc failure!");
  }// end if creating a copy for grabbing number of arguments

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

  return position;
}// end function split_line

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
  return tokens;
}// end function split_line


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
  Determine if all the threads are done running
*/
int all_threads_done(int *threads_done, int num_threads)
{
  int i;
  
  for(i = 0; i < num_threads; i++){     
     if(threads_done[i] == 0){
	return 0;;
     }// end if one of the threads isn't done	
  }// end for loop over all the threads
  return 1;
}// end function all_threads_done

/*
  Loop waiting for worker threads to finish
*/
void wait_for_workers(int *threads_done, int num_worker_threads)
{
  while(!all_threads_done(threads_done, num_worker_threads)){ /* wait until all threads are done */ }
}// end function wait_for_workers

