#ifndef QUEUE
#define QUEUE	

// A structure to represent an individual request
struct Request
{
  int accountID;
  int requestID;
  int value;
  int write;
};

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    struct Request* array;
};

// function to create a new request to be serviced by worker thread
struct Request* createRequest(int requestID, int accountID, int value, int write);

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity);

// Function to determine if queue is full
int isFull(struct Queue* queue);

// Queue is empty when size is 0
int isEmpty(struct Queue* queue);

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, struct Request *item);

// Function to remove an item from queue. 
// It changes front and size
struct Request *dequeue(struct Queue* queue);

// Function to get front of queue
struct Request *front(struct Queue* queue);

// Function to get rear of queue
struct Request *rear(struct Queue* queue);

#endif
