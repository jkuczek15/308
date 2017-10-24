// C program for array implementation of queue
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "queue.h"

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (struct Request*) malloc(queue->capacity * sizeof(struct Request));
    return queue;
}

// function to create a new request to be serviced by worker thread
struct Request* createRequest(int accountID, int value, int write)
{
    struct Request* request = (struct Request*) malloc(sizeof(struct Request));
    request->accountID = accountID;
    request->value = value; 
    request->write = write;
    return request;
}

// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{  return (queue->size == queue->capacity);  }

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{  return (queue->size == 0); }

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, struct Request *item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = *item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue. 
// It changes front and size
struct Request *dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    struct Request *item = &(queue->array[queue->front]);
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
struct Request *front(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return &(queue->array[queue->front]);
}

// Function to get rear of queue
struct Request *rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return &(queue->array[queue->rear]);
}

