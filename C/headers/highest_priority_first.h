#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
#include "message_buffer.h"
#include <math.h>
#include "buddy_algorithm.h"

freeList freeArr;
char memoryArr[1024];

void HPF(vector *v, int msgqid1, int msgqid2) {

  int shmk_buddy = ftok("key_buddy", 10);
  int shmid_buddy = shmget(shmk_buddy, 1024, IPC_CREAT | 0666);

  initialize_shm(shmid_buddy,&memoryArr);
  initializeFreeList(&freeArr);
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
    printf("%d \n",time);

    for (int i=0;i < size(v);i++) {

      // Skip the finished processes
      if (v->array[i].state == 'F') {
        continue;
      }

      else {   // State == N
        //printf("size %d \n",v->array[i].memorySize+16);
        // Check if the not finished process arrived
        if(v->array[i].arrivalTime <= time) {
          pair memPosition = allocate(&freeArr,&memoryArr,v->array[i].memorySize);
          //printf("poss %d %d \n",memPosition.start,memPosition.end);

          if(memPosition.start == -1 && memPosition.end == -1) return;
          else {
            v->array[i].memoryStartIndex = memPosition.start;
            v->array[i].memoryEndIndex = memPosition.end;
          }
          int memSize = memPosition.end -memPosition.start;
          // Update process start time
          v->array[i].state = 'S';
          v->array[i].startTime = time;
          v->array[i].waitTime = time - v->array[i].arrivalTime;
          int firstTime =1;

          fprintf(outFile,"At time %d allocatd %d bytes for process %d from %d to %d\n",time,memSize,v->array[i].ID,memPosition.start,memPosition.end);
          while(getClk()-time< v->array[i].burstTime){
            if(firstTime==1) {
              firstTime = 0;
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
            else {
              kill(v->array[i].pid, SIGCONT );
              message.mtype = v->array[i].pid;
              msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
              message.mtype = getpid();
              msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
              message.mtype = getpid();
              kill(v->array[i].pid, SIGSTOP);
            }
          }
          kill(v->array[i].pid, SIGCONT );
          int state;
          waitpid(v->array[i].pid, &state, (int)NULL);
          // Update the process info
          v->array[i].finishTime = getClk();
          v->array[i].waitTime = v->array[i].finishTime-v->array[i].arrivalTime;
          v->array[i].state = 'F';
          // Increment the processes counter
          finished+=1;
          deallocate(&freeArr,&memoryArr,v->array[i].memoryStartIndex,v->array[i].memoryEndIndex);
          fprintf(outFile,"At time %d freed %d bytes from process %d form %d to %d\n",v->array[i].finishTime,memSize,v->array[i].ID,memPosition.start, memPosition.end);
          printf("helllo\n");
          break;
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