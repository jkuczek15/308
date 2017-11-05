/*******************************************************************************
*
* CprE 308 Scheduling Lab
*
* scheduling.c
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_PROCESSES 20

struct process
{
  /* Values initialized for each process */
  int arrivaltime;  /* Time process arrives and wishes to start */
  int runtime;      /* Time process requires to complete job */
  int priority;     /* Priority of the process */

  /* Values algorithm may use to track processes */
  int starttime;
  int endtime;
  int flag;
  int remainingtime;
} process;

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
void round_robin(struct process *proc);
void round_robin_priority(struct process *proc);

int main()
{
  int i;
  struct process proc[NUM_PROCESSES],      /* List of processes */
                 proc_copy[NUM_PROCESSES]; /* Backup copy of processes */

  /* Seed random number generator */
  /*srand(time(0));*/  /* Use this seed to test different scenarios */
  srand(0xC0FFEE);     /* Used for test to be printed out */

  /* Initialize process structures */
  for(i = 0; i < NUM_PROCESSES; i++)
  {
    proc[i].arrivaltime = rand() % 100;
    proc[i].runtime = (rand() % 30) + 10;
    proc[i].priority = rand() % 3;
    proc[i].starttime = 0;
    proc[i].endtime = 0;
    proc[i].flag = 0;
    proc[i].remainingtime = 0;
  }// end for loop over num_processes

  /* Show process values */
  printf("Process\tarrival\truntime\tpriority\n");
  for(i=0; i<NUM_PROCESSES; i++)
    printf("%d\t%d\t%d\t%d\n", i, proc[i].arrivaltime, proc[i].runtime,
           proc[i].priority);

  /* Run scheduling algorithms */
  printf("\n\nFirst come first served\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  first_come_first_served(proc_copy);

  printf("\n\nShortest remaining time\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  shortest_remaining_time(proc_copy);

  printf("\n\nRound Robin\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin(proc_copy);

  printf("\n\nRound Robin with priority\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin_priority(proc_copy);

  return 0;
}// end main function

void first_come_first_served(struct process *proc)
{
  int i, minIndex;
  int count = 0;
  int time = 0;
  int total_difference = 0;
  struct process;

  while(count != NUM_PROCESSES)
  {
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      if(!proc[i].flag){
        minIndex = i;
      }// end if process not flagged
    }// end for loop picking 1st unflagged process

    for(i = NUM_PROCESSES-1; i >= 0; i--)
    {
      if(!proc[i].flag && proc[i].arrivaltime <= proc[minIndex].arrivaltime){
        minIndex = i;
      }// end if this is the next minimum process

    }// end for loop over num_processes

    process = proc[minIndex];

    if(time >= process.arrivaltime){
      process.starttime = time;
    }else{
      process.starttime = process.arrivaltime;
    }// end if running time is greater than arrival time

    // set the endtime, keep track of arrival to finish
    process.endtime = process.starttime + process.runtime;
    total_difference += process.endtime - process.arrivaltime;
    
    // set the current time to be the endtime of this process
    time = process.endtime;
    proc[minIndex].flag = 1;

    // output the results
    printf("Process %d started at time %d\n", minIndex, process.starttime);
    printf("Process %d finished at time %d\n", minIndex, process.endtime);
    count++;
  }// end while loop printing out process information

  printf("Average time from arrival to finish is %d seconds\n", total_difference / NUM_PROCESSES);
}// end function first_come_first_served

void shortest_remaining_time(struct process *proc)
{
  int i, minIndex;
  int count = 0;
  int time = 0;
  int total_difference = 0;
  struct process;

  while(count != NUM_PROCESSES)
  {
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      if(!proc[i].flag){
        minIndex = i;
      }// end if process not flagged
    }// end for loop picking 1st unflagged process

    for(i = NUM_PROCESSES-1; i >= 0; i--)
    {
      if(!proc[i].flag && proc[i].runtime <= proc[minIndex].runtime){
        minIndex = i;
      }// end if this is the next minimum process
    }// end for loop over num_processes

    process = proc[minIndex];

    if(time >= process.arrivaltime){
      process.starttime = time;
    }else{
      process.starttime = process.arrivaltime;
    }// end if running time is greater than arrival time

    // set the endtime, keep track of arrival to finish
    process.endtime = process.starttime + process.runtime;
    total_difference += process.endtime - process.arrivaltime;

    // set the current time to be the endtime of this process
    time = process.endtime;
    proc[minIndex].flag = 1;

    // output the results
    printf("Process %d started at time %d\n", minIndex, process.starttime);
    printf("Process %d finished at time %d\n", minIndex, process.endtime);
    count++;
  }// end while loop printing out process information

  printf("Average time from arrival to finish is %d seconds\n", total_difference / NUM_PROCESSES);
}// end function shortest_remaining_time

void round_robin(struct process *proc)
{
  int count = 0;
  int time = 0;
  int quantum = 1;
  struct process;
  
  while(count != NUM_PROCESSES)
  {
    process = proc[count];
    process.remainingtime = process.runtime;

    if(time >= process.arrivaltime){
      process.starttime = time;
    }else{
      process.starttime = process.arrivaltime;
    }// end if running time is greater than arrival time

    if(!process.flag){
      printf("Process %d started at time %d\n", count, process.starttime);
      process.flag = 1;
    }else{
      
    }

    process.endtime = process.starttime + quantum;
    process.remainingtime = process.runtime - quantum;

    if(process.remainingtime <= 0){
      printf("Process %d finished at time %d\n", count, process.endtime);
      count++;
    }

    
    
    time = process.endtime;
    
    count++;
  }// end while loop printing out process information

  printf("Average time from arrival to finish is %d seconds\n", time / NUM_PROCESSES);
}// end function round_robin

void round_robin_priority(struct process *proc)
{
  /* Implement scheduling algorithm here */
}// end function round_robin_priority

