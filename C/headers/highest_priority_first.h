#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "headers.h"
#include "message_buffer.h"

void HPF(vector *v, int msgqid1, int msgqid2) {
  // Sort the vector w.r.t. priority
  sort(v, "priority");

  FILE *outFile = fopen("scheduler.log", "w");
  fprintf(outFile,"#At time x process y state arr w total z remain y wait k \n");

  // Message Decleration
  struct msgbuff message;

  // Initilize clk
  initClk();

  // number of finished processes
  int finished = 0;
  int time = getClk();
  // Loop to serve all processes
  while( finished != size(v)) {
    // Wait until the clk initialized (for the first time)
    while(time == -1);
    time = getClk();
    for (int i=0;i < size(v);i++) {
      // Skip the finished processes
      if (v->array[i].state == 'F') {
        continue;
      }
      else {   // State == N
        // Check if the not finished process arrived
        if(v->array[i].arrivalTime <= time) {
          // Update process start time
          v->array[i].state = 'S';
          v->array[i].startTime = time;
          v->array[i].waitTime = time - v->array[i].arrivalTime;
          fprintf(outFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",time,v->array[i].ID,v->array[i].arrivalTime,v->array[i].burstTime,v->array[i].burstTime,v->array[i].waitTime);
          while(getClk()-time< v->array[i].burstTime){
            // Serve the process
            int pid = fork();
            if(pid == 0) {
              // Serve process
              char rem[100];
              sprintf(rem, "%d", v->array[i].burstTime);
              execlp("./process.out", "./process.out", rem, NULL);
            }
            else {
              v->array[i].pid = pid;
              message.mtype = pid;
              msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
              message.mtype = getpid();
              msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
              message.mtype = getpid();
              kill(pid, SIGSTOP);
            }
          }
          // Update the process info
          v->array[i].finishTime = getClk();
          time = v->array[i].finishTime;
          v->array[i].state = 'F';
          // Increment the processes counter
          finished+=1;
          int state;
          waitpid(v->array[i].pid, &state, (int)NULL);
          fprintf(outFile,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f \n",time,v->array[i].ID,v->array[i].arrivalTime,v->array[i].burstTime,0,v->array[i].waitTime,v->array[i].finishTime-v->array[i].arrivalTime,(v->array[i].finishTime-v->array[i].arrivalTime)/(v->array[i].burstTime*1.0));
        }
        // If the process not finished and not arrived too skip it
        else {
          continue;
        }
      }
    }
  }
 fclose(outFile);
}