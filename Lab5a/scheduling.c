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

int total = 0;
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
  int started;
  int finished;
} process;

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
int round_robin(struct process *proc, int priority, int startTime);
void round_robin_priority(struct process *proc);

/* Forward declarations of other functions */
int getMin(struct process *proc, int runtime);
int getNumProcs(struct process *proc, int priority);

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
    proc[i].started = 0;
    proc[i].finished = 0;
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
  round_robin(proc_copy, -1, 1);

  printf("\n\nRound Robin with priority\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin_priority(proc_copy);

  return 0;
}// end main function

void first_come_first_served(struct process *proc)
{
  int minIndex;
  int count = 0;
  int time = 0;
  int total_difference = 0;
  struct process *process;

  while(count != NUM_PROCESSES)
  {
    minIndex = getMin(proc, 0);
    process = &(proc[minIndex]);

    if(time >= process->arrivaltime){
      process->starttime = time;
    }else{
      process->starttime = process->arrivaltime;
    }// end if running time is greater than arrival time

    // set the endtime, keep track of arrival to finish
    process->endtime = process->starttime + process->runtime;
    total_difference += process->endtime - process->arrivaltime;
    
    // set the current time to be the endtime of this process
    time = process->endtime;
    proc[minIndex].flag = 1;

    // output the results
    printf("Process %d started at time %d\n", minIndex, process->starttime);
    printf("Process %d finished at time %d\n", minIndex, process->endtime);
    count++;
  }// end while loop printing out process information

  printf("Average time from arrival to finish is %d seconds\n", total_difference / NUM_PROCESSES);
}// end function first_come_first_served

void shortest_remaining_time(struct process *proc)
{
  int minIndex;
  int count = 0;
  int time = 0;
  int total_difference = 0;
  struct process *process;

  while(count != NUM_PROCESSES)
  {
    minIndex = getMin(proc, 1);
    process = &(proc[minIndex]);

    if(time >= process->arrivaltime){
      process->starttime = time;
    }else{
      process->starttime = process->arrivaltime;
    }// end if running time is greater than arrival time

    // set the endtime, keep track of arrival to finish
    process->endtime = process->starttime + process->runtime;
    total_difference += process->endtime - process->arrivaltime;

    // set the current time to be the endtime of this process
    time = process->endtime;
    proc[minIndex].flag = 1;

    // output the results
    printf("Process %d started at time %d\n", minIndex, process->starttime);
    printf("Process %d finished at time %d\n", minIndex, process->endtime);
    count++;
  }// end while loop printing out process information

  printf("Average time from arrival to finish is %d seconds\n", total_difference / NUM_PROCESSES);
}// end function shortest_remaining_time

int round_robin(struct process *proc, int priority, int startTime)
{
  int i;
  int total_difference = 0;
  int count = 0;
  int time = startTime;
  int quantum = 10;
  int num_procs_with_priority;
  struct process *process;
  
  if(priority == -1){
    num_procs_with_priority = NUM_PROCESSES;
  }else{
    num_procs_with_priority = getNumProcs(proc, priority);
  }// end if priority == -1
  
  while(count != num_procs_with_priority)
  {
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      process = &(proc[i]);
 
      if(process->finished || time < process->arrivaltime || (priority != -1 && process->priority != priority)){
        // process has not arrived, or it's done, keep going
        if(priority != -1){
          time ++;
        }// end if priority != -1
        continue;
      }// end if the process hasn't arrived yet
      
      if(!process->started){
        // process is being started
        printf("Process %d started at time %d\n", i, time);
        process->remainingtime = process->runtime - quantum;
        process->started = 1;
      }else{
        // process is currently running
        process->remainingtime -= quantum;
      }// end if the process is flagged
      
      // increment the time counter
      time += quantum;

      if(process->remainingtime <= 0){
        time += process->remainingtime;
        printf("Process %d finished at time %d\n", i, time);
        total_difference += time - process->arrivaltime;
        process->finished = 1;
        count++;
      }// end if process is done running
      
      if(count == NUM_PROCESSES){
        break;
      }// end if we've reached the end

    }// end for loop over process order

  }// end while loop printing out process information

  if(priority == -1){
    printf("Average time from arrival to finish is %d seconds\n", total_difference / NUM_PROCESSES);
  }// end if priority == -1

  total += total_difference;

  if(priority == 0){
    printf("Average time from arrival to finish is %d seconds\n", total / NUM_PROCESSES);
  }// end if priority == 0
  return time;
}// end function round_robin

void round_robin_priority(struct process *proc)
{
  int time;
  // run the round robin with all three priorities
  time = round_robin(proc, 2, 1);
  time = round_robin(proc, 1, time);
  round_robin(proc, 0, time);
}// end function round_robin_priority

int getMin(struct process *proc, int runtime)
{
  int minIndex, i;
  for(i = 0; i < NUM_PROCESSES; i++)
  {
    if(!proc[i].flag){
      minIndex = i;
      break;
    }// end if process not flagged
  }// end for loop picking 1st unflagged process

  for(i = NUM_PROCESSES-1; i >= 0; i--)
  {
    if(!proc[i].flag)
    {
      if(runtime && proc[i].runtime <= proc[minIndex].runtime){
        minIndex = i;
      }else if(proc[i].arrivaltime <= proc[minIndex].arrivaltime){
        minIndex = i;
      }// end if getting the minimum runtime
    }// end if the process is not flagged
  }// end for loop over num_processes

  return minIndex;
}// end function getMin()

int getNumProcs(struct process *proc, int priority)
{
  int count = 0;
  int i;

  for(i = 0; i < NUM_PROCESSES; i++){
    if(proc[i].priority == priority){
      count++;
    }// end if the two priorities match
  }// end for loop over processes
  return count;
}// end function getNumProces
