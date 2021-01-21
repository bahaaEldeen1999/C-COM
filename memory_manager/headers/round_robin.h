#include "vector.h"
#include "integer_queue.h"
#include "headers.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "message_buffer.h"
#include "buddy_algorithm.h"

void roundRobin(vector *process, unsigned int quantum, int msgqid1, int msgqid2, char *memoryArr)
{
    // initialze free list to keep track of memeoy allocations amd deallocations in buddy algorithm
    freeList freeArr;
    initializeFreeList(&freeArr);

    FILE *memory_log = fopen("memory.log", "w");
    fprintf(memory_log, "#At time x allocated y bytes for process z from i to j \n");

    FILE *scheduler_log = fopen("./scheduler.log", "w");
    FILE *scheduler_perf = fopen("./scheduler.perf", "w");
    fprintf(scheduler_log, "#At time x process y state arr w total z remain y wait k \n");
    // the round robin queue
    vector q;
    initialize(&q, 0);
    // initilaze helpful variables to calculate the perf file
    float expectedTime = 0;
    float trueTime = 0;
    float totalWTA = 0;
    float totalWait = 0;
    int numOfProcess = size(process);
    float *WTA = (float *)malloc(sizeof(float) * (numOfProcess + 1));
    int time = getClk();
    struct msgbuff message;
    int curr_index = 0;
    float startTime = getClk();
    while (size(&q) > 0 || curr_index < size(process))
    {
        time = getClk();
        //printf("time %d\n", time);
        int temp = curr_index;
        for (int i = temp; i < size(process); i++)
        {
            if (get(process, i).arrivalTime <= time)
            {
                push(&q, get(process, i));
                curr_index++;
            }
            else
            {
                break;
            }
        }
        if (size(&q) == 0)
        {
            continue;
        }
        PCB p = top(&q);
        pop(&q);
        int rem_time = p.remainingTime;
        int firstTime = 0;
        if (p.startTime == -1)
        {

            // Assign memory
            pair memPosition = allocate(&freeArr, memoryArr, p.memorySize);
            if (memPosition.start == -1 && memPosition.end == -1)
            {
                // if no memory left add the process to end of ready queue so when a free place for it is found we start assigning it to the memory
                push(&q, p);
                continue;
            }
            else
            {

                //Update process info
                p.memoryStartIndex = memPosition.start;
                p.memoryEndIndex = memPosition.end;
            }

            int memSize = memPosition.end - memPosition.start + 1;
            fprintf(memory_log, "At time %d allocated %d bytes for process %d from %d to %d\n", time, memSize, p.ID, memPosition.start, memPosition.end);
            firstTime = 1;
            p.waitTime = time - p.arrivalTime;
            p.startTime = time;
            fprintf(scheduler_log, "At time %d process %d started arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, p.burstTime, p.waitTime);
        }
        else
        {
            p.waitTime += time - p.lastRunTime;
            fprintf(scheduler_log, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime);
        }
        if (rem_time > quantum)
        {

            while (getClk() - time < quantum)
            {
                //printf("clk %d\n", getClk());
                // check if first time then fork the process and run it for d=first time
                if (firstTime == 1)
                {

                    firstTime = 0;
                    int pid = fork();
                    if (pid == 0)
                    {
                        // execute the process.o
                        char rem[100];
                        sprintf(rem, "%d", p.remainingTime);
                        execlp("./process.out", "./process.out", rem, NULL);
                    }
                    else
                    {
                        // set process pid to pid
                        p.pid = pid;
                        message.mtype = p.pid;
                        msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
                        message.mtype = getpid();
                        msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
                        message.mtype = getpid();
                        kill(p.pid, SIGSTOP);
                    }
                }
                else
                {
                    // resume process
                    //printf("cont %d \n", getClk());
                    kill(p.pid, SIGCONT);
                    message.mtype = p.pid;
                    msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
                    message.mtype = getpid();
                    msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
                    message.mtype = getpid();
                    //printf("rec %d \n", getClk());
                    kill(p.pid, SIGSTOP);
                }
            }
            trueTime += (getClk() - time);
            expectedTime += quantum;
            rem_time -= quantum;
            time = getClk();
        }
        else
        {
            while (getClk() - time < rem_time)
            {
                //printf("clk %d\n", getClk());
                // check if first time then fork the process and run it for d=first time
                if (firstTime == 1)
                {
                    firstTime = 0;
                    int pid = fork();
                    if (pid == 0)
                    {

                        char rem[100];
                        sprintf(rem, "%d", p.remainingTime);
                        int x = execl("./process.out", "./process.out", rem, NULL);
                    }
                    else
                    {
                        // set process pid to pid
                        p.pid = pid;
                        message.mtype = p.pid;
                        msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
                        message.mtype = getpid();
                        msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
                        message.mtype = getpid();
                        kill(p.pid, SIGSTOP);
                    }
                }
                else
                {
                    // resume process
                    // printf("cont2 %d \n", getClk());
                    kill(p.pid, SIGCONT);
                    message.mtype = p.pid;
                    msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
                    message.mtype = getpid();
                    msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
                    //printf("rec2 %d \n", getClk());
                    message.mtype = getpid();
                    kill(p.pid, SIGSTOP);
                }
            }
            //printf("wait for orocess to die\n");

            trueTime += (getClk() - time);
            expectedTime += rem_time;
            time = getClk();
            rem_time = 0;
            int state;
            kill(p.pid, SIGCONT);
            waitpid(p.pid, &state, (int)NULL);
        }
        temp = curr_index;
        for (int i = temp; i < size(process); i++)
        {
            if (get(process, i).arrivalTime <= time)
            {
                push(&q, get(process, i));
                curr_index++;
            }
            else
            {
                break;
            }
        }
        if (rem_time > 0)
        {

            p.lastRunTime = time;
            p.remainingTime = rem_time;
            process->array[p.index].lastRunTime = time;
            process->array[p.index].startTime = p.startTime;
            process->array[p.index].remainingTime = p.remainingTime;

            push(&q, p);
            fprintf(scheduler_log, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime);
        }
        else
        {

            process->array[p.index].finishTime = time;
            WTA[p.index] = (1.0 * (time - p.arrivalTime)) / (float)(1.0 * p.burstTime);
            fprintf(scheduler_log, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime, time - p.arrivalTime, (1.0 * (time - p.arrivalTime)) / (float)(1.0 * p.burstTime));

            totalWait += p.waitTime;
            totalWTA += (1.0 * (time - p.arrivalTime)) / (float)(1.0 * p.burstTime);

            deallocate(&freeArr, memoryArr, p.memoryStartIndex, p.memoryEndIndex);
            int memSize = p.memoryEndIndex - p.memoryStartIndex + 1;
            fprintf(memory_log, "At time %d freed %d bytes from process %d from %d to %d\n", time, memSize, p.ID, p.memoryStartIndex, p.memoryEndIndex);
        }
    }

    // calculating perf file
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
    fclose(scheduler_log);
    fclose(scheduler_perf);
    fclose(memory_log);
}
