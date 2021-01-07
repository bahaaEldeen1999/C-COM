#pragma once
#include <stdio.h>
#include <stdlib.h>
typedef struct
{
      __uint32_t arrivalTime, burstTime, priority, ID, startTime, finishTime, lastRunTime, remainingTime, waitTime, pindx;
      __u_char state;
      pid_t pid;
} PCB;

// -------------------------------------Helper Functions---------------------------------------------

void equalize(PCB *x, PCB *y)
{
      x->arrivalTime = y->arrivalTime;
      x->burstTime = y->burstTime;
      x->startTime = y->startTime;
      x->finishTime = y->finishTime;
      x->lastRunTime = y->lastRunTime;
      x->remainingTime = y->remainingTime;
      x->waitTime = y->waitTime;
      x->pid = y->pid;
      x->ID = y->ID;
      x->priority = y->priority;
      x->state = y->state;
}

void swap(PCB *x, PCB *y)
{
      PCB tmp;
      equalize(&tmp, y);
      equalize(y, x);
      equalize(x, &tmp);
}

__int32_t compare(PCB *x, PCB *y, char sortingKey[])
{
      if (sortingKey == "arrivalTime")
            return (x->arrivalTime - y->arrivalTime);
      else if (sortingKey == "burstTime")
            return (x->burstTime - y->burstTime);
      else if (sortingKey == "startTime")
            return (x->startTime - y->startTime);
      else if (sortingKey == "finishTime")
            return (x->finishTime - y->finishTime);
      else if (sortingKey == "lastRunTime")
            return (x->lastRunTime - y->lastRunTime);
      else
            return (x->priority - y->priority);
}

void printPCB(PCB x)
{
      printf("#id arrival burst priority start finish state\n");
      printf("%d\t%d\t%d\t%d\t%d\t%d\t%c\n", x.ID, x.arrivalTime, x.burstTime, x.priority, x.startTime, x.finishTime, x.state);
}