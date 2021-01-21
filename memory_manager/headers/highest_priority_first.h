#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
#include "message_buffer.h"
#include <math.h>
#include "buddy_algorithm.h"

void HPF(vector *v, int msgqid1, int msgqid2, char *memoryArr)
{

  // Free blocks array used for buddy_algorithm
  freeList freeArr;
  initializeFreeList(&freeArr);

  // Sort the vector w.r.t. priority
  sort(v, "priority");

  // Init memory file
  FILE *outFile = fopen("memory.log", "w");
  fprintf(outFile, "#At time x allocated y bytes for process z from i to j \n");

  // Init scheduler file
  FILE *scheduler = fopen("scheduler.log", "w");
  fprintf(scheduler, "#At time x process y state arr w total z remain y wait k \n");

  // Message Decleration for communication between process and scheduler
  struct msgbuff message;

  // Initilize clk
  initClk();

  // number of finished processes
  int finished = 0;

  //Store the current clock at time
  int time = getClk();

  // Some variables for .pref calculations
  float startTime = getClk();
  float expectedTime = 0;
  float trueTime = 0;
  float totalWTA = 0;
  float totalWait = 0;
  int numOfProcess = size(v);
  float *WTA = (float *)malloc(sizeof(float) * (numOfProcess + 1));

  // Loop to serve all processes
  while (finished != size(v))
  {

    // Wait until the clk initialized (for the first time)
    while (time == -1)
      ;

    // Update time
    time = getClk();
    for (int i = 0; i < size(v); i++)
    {

      // Skip the finished processes
      if (v->array[i].state == 'F')
      {
        continue;
      }

      else
      {
        // State == N
        // Check if the not finished process arrived
        if (v->array[i].arrivalTime <= time)
        {
          pair memPosition = allocate(&freeArr, memoryArr, v->array[i].memorySize);

          if (memPosition.start == -1 && memPosition.end == -1)
            break;
          else
          {
            v->array[i].memoryStartIndex = memPosition.start;
            v->array[i].memoryEndIndex = memPosition.end;
          }
          int memSize = memPosition.end - memPosition.start + 1;
          // Update process info
          v->array[i].state = 'S';
          v->array[i].startTime = time;
          v->array[i].waitTime = time - v->array[i].arrivalTime;
          int firstTime = 1;

          expectedTime += v->array[i].burstTime;

          fprintf(outFile, "At time %d allocated %d bytes for process %d from %d to %d\n", time, memSize, v->array[i].ID, memPosition.start, memPosition.end);
          fprintf(scheduler, "At time %d process %d started arr %d total %d remain %d wait %d\n", time, v->array[i].ID, v->array[i].arrivalTime, v->array[i].burstTime, v->array[i].burstTime, v->array[i].waitTime);

          while (getClk() - time < v->array[i].burstTime)
          {
            if (firstTime == 1)
            {
              firstTime = 0;
              int pid = fork();

              if (pid == 0)
              {
                // Serve process
                char rem[100];
                sprintf(rem, "%d", v->array[i].burstTime);
                execlp("./process.out", "./process.out", rem, NULL);
              }
              else
              {
                v->array[i].pid = pid;
                message.mtype = pid;
                msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
                message.mtype = getpid();
                msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
                message.mtype = getpid();
                kill(pid, SIGSTOP);
              }
            }
            else
            {
              kill(v->array[i].pid, SIGCONT);
              message.mtype = v->array[i].pid;
              msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
              message.mtype = getpid();
              msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
              message.mtype = getpid();
              kill(v->array[i].pid, SIGSTOP);
            }
          }
          kill(v->array[i].pid, SIGCONT);
          int state;
          waitpid(v->array[i].pid, &state, (int)NULL);
          // Update the process info
          v->array[i].finishTime = getClk();
          v->array[i].state = 'F';
          // Increment the processes counter
          finished += 1;
          int TA = v->array[i].finishTime - v->array[i].arrivalTime;

          // Free the process memory
          deallocate(&freeArr, memoryArr, v->array[i].memoryStartIndex, v->array[i].memoryEndIndex);

          // out the info to the files
          fprintf(scheduler, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f \n", v->array[i].finishTime, v->array[i].ID, v->array[i].arrivalTime, v->array[i].burstTime, 0, v->array[i].waitTime, TA, TA / (v->array[i].burstTime * 1.0));
          fprintf(outFile, "At time %d freed %d bytes from process %d from %d to %d\n", v->array[i].finishTime, memSize, v->array[i].ID, v->array[i].memoryStartIndex, v->array[i].memoryEndIndex);
          // .pref calculations
          trueTime += (getClk() - time);
          totalWait += v->array[i].waitTime;
          WTA[i] = TA / (v->array[i].burstTime * 1.0);
          totalWTA += TA / (v->array[i].burstTime * 1.0);
          break;
        }
        // If the process not finished and not arrived too skip it
        else
        {
          continue;
        }
      }
    }
  }

  fclose(outFile);
  fclose(scheduler);

  // calculating perf file
  FILE *scheduler_perf = fopen("scheduler.perf", "w");
  float totalTime = getClk() - startTime;
  float avgWTA = totalWTA / numOfProcess;
  float avgWait = totalWait / numOfProcess;
  float cpuUtilize = (expectedTime / totalTime) * 100;
  float stdWTA = 0;
  for (int i = 0; i < numOfProcess; i++)
  {
    stdWTA += (WTA[i] - avgWTA) * (WTA[i] - avgWTA);
  }
  stdWTA /= numOfProcess;
  stdWTA = sqrt(stdWTA);
  ///////////////////////
  fprintf(scheduler_perf, "CPU utilization = %.2f%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nstd WTA = %.2f", cpuUtilize, avgWTA, avgWait, stdWTA);
  fclose(scheduler_perf);
}