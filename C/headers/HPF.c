#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "headers.h"

void HPF(vector *v) {
  printf("%d",size(&v));

  // Sort the vector w.r.t. priority
  sort(&v, "priority");
  printVector(&v);

  printf("%d",size(&v));

  // Get the current clk
  int time = getClk();

  FILE *file = fopen("schedulr.log", "w");
  fputs("#At time x process y state arr w total z remain y wait k \n", file);
  unsigned int finished = 0;
  while( finished != size(&v)) {
    for (int i=0;i < size(&v);i++) {
      if (v->array[i].state == 'F') {
        continue;
      }
      else {   // State == N
        if(v->array[i].arrivalTime < time) {
          v->array[i].startTime = getClk();
          fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),v->array[i].ID,v->array[i].arrivalTime,v->array[i].burstTime,v->array[i].burstTime,getClk()-v->array[i].arrivalTime);
          // Serve the process
          while(getClk() < v->array[i].startTime + v->array[i].burstTime) {}
          // Update the process info
          v->array[i].state = 'F';
          v->array[i].finishTime = getClk();
          finished+=1;
          fprintf(file,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2d \n",getClk(),v->array[i].ID,v->array[i].arrivalTime,v->array[i].burstTime,0,v->array[i].startTime-v->array[i].arrivalTime,v->array[i].finishTime-v->array[i].arrivalTime,(v->array[i].finishTime-v->array[i].arrivalTime)/v->array[i].burstTime);
        }
        else {
          continue;
        }
      }
    }
  }
}

int main() {
    vector process;
    printf("initialze process vector\n");
    initialize(&process, 4);
    PCB p;
    p.state = 'N';
    p.ID = 2;
    p.arrivalTime = 1;
    p.burstTime = 6;
    p.startTime = 0;
    p.priority=2;
    p.finishTime=0;
    p.lastRunTime=0;
    p.remainingTime=0;
    p.waitTime=0;
    set(&process, 0, p);
    p.state = 'N';
    p.ID = 3;
    p.arrivalTime = 6;
    p.burstTime = 4;
    p.priority=3;
    p.startTime = 0;
    p.finishTime=0;
    p.lastRunTime=0;
    p.remainingTime=0;
    p.waitTime=0;
    set(&process, 1, p);
    p.state = 'N';
    p.ID = 1;
    p.priority=0;
    p.arrivalTime = 7;
    p.burstTime = 1;
    p.startTime = 0;
    p.finishTime=0;
    p.lastRunTime=0;
    p.remainingTime=0;
    p.waitTime=0;
    set(&process, 2, p);
    p.state = 'N';
    p.ID = 4;
    p.arrivalTime = 0;
    p.burstTime = 2;
    p.priority=4;
    p.startTime = 0;
    p.finishTime=0;
    p.lastRunTime=0;
    p.remainingTime=0;
    p.waitTime=0;
    set(&process, 3, p);
    printf("HPF\n");
    initClk();
    HPF(&process);
    destroyClk(true);
    return 0;
}