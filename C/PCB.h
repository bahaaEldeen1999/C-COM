#include <stdlib.h>

typedef struct
{
      __uint32_t arrivalTime, burstTime, priority, ID, startTime, finishTime, lastRunTime;
      __u_char state;
} PCB;

void equalize(PCB *x, PCB *y)
{
      x->arrivalTime = y->arrivalTime;
      x->burstTime = y->burstTime;
      x->startTime = y->startTime;
      x->finishTime = y->finishTime;
      x->lastRunTime = y->lastRunTime;
      x->ID = y->ID;
      x->priority = y->priority;
      x->state = y->state;
}

// __uint32_t xlessThanOrEqualy(const void *a, const void *b)
// {
//       PCB *x = (PCB *)a;
//       PCB *y = (PCB *)b;
//       return x->priority - y->priority;
// }

// __uint32_t xgreaterThanOrEqualy(const void *a, const void *b)
// {
//       PCB *x = (PCB *)a;
//       PCB *y = (PCB *)b;
//       return y->priority - x->priority;
// }