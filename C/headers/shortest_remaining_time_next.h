#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "vector.h"
#include "headers.h"
#include "string.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "message_buffer.h"

char output_path[] = "scheduler.log";

void action_log(FILE *file, PCB *item, char *statement, int clk)
{
    int waiting_time = clk + item->remainingTime - (item->arrivalTime + item->burstTime);

    if (statement == "finished")
    {
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %lf\n",
                clk, item->ID, statement, item->arrivalTime, item->burstTime,
                item->remainingTime, waiting_time, item->burstTime + waiting_time,
                ((double)item->burstTime + waiting_time) / (item->burstTime));
    }
    else
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                clk, item->ID, statement, item->arrivalTime, item->burstTime, item->remainingTime, waiting_time);
}

int create_new_process(PCB *item)
{
    int child_pid = fork();

    if (child_pid != 0)
    {
        return child_pid;
    }
    else
    {
        char casting[10];
        sprintf(casting, "%d", item->remainingTime);
        //printf("here exec\n");
        int err = execl("./process.out", "./process.out", casting, NULL);
        //printf("err %d\n", err);
    }
}

void SRTN(vector *v, int msgq_id1, int msgq_id2, char *memoryArr)
{
    //printf("***** SRTN *****\n");
    freeList freeArr;
    initializeFreeList(&freeArr);
    int n = size(v);

    // Open files
    FILE *file = fopen(output_path, "w");
    FILE *scheduler_perf = fopen("./scheduler.perf", "w");
    FILE *memory_log = fopen("./memory.log", "w");

    fprintf(file, "#At time x process y state arr w total z remain y wait k \n");
    fprintf(memory_log, "#At time x allocated y bytes for process z form i to j \n");

    // Init "gotresponse" vector for the comming to work processes
    vector *gotresponse = (vector *)malloc(0);
    initialize(gotresponse, 0);
    int cur_proc = -1;

    struct msgbuff msg;

    // calc perf
    float expectedTime = 0;
    float trueTime = 0;
    float totalWTA = 0;
    float totalWait = 0;

    int first_clk = getClk();
    int count_done = 0;

    //////////////////////////////
    float *WTA = (float *)malloc(sizeof(float) * (n + 1));
    while (size(v) || size(gotresponse))
    {
        int last_clk = getClk();
        //printf("CLK is : %d\n", getClk());

        if (cur_proc != -1 && gotresponse->array[cur_proc].remainingTime == 0)
        {
            // Done Process
            int state;
            kill(gotresponse->array[cur_proc].pid, SIGCONT);
            waitpid(gotresponse->array[cur_proc].pid, &state, (int)NULL);

            // Free memory and report about it
            PCB p = gotresponse->array[cur_proc];
            deallocate(&freeArr, memoryArr, p.memoryStartIndex, p.memoryEndIndex);
            int memSize = p.memoryEndIndex - p.memoryStartIndex + 1;
            fprintf(memory_log, "At time %d freed %d bytes from process %d form %d to %d\n", getClk(), memSize, p.ID, p.memoryStartIndex, p.memoryEndIndex);

            // Report about the done process
            action_log(file, &gotresponse->array[cur_proc], "finished", getClk());

            // calc about .perf
            int waiting_time = getClk() + gotresponse->array[cur_proc].remainingTime - (gotresponse->array[cur_proc].arrivalTime + gotresponse->array[cur_proc].burstTime);

            int curWTA = ((float)gotresponse->array[cur_proc].burstTime + waiting_time) / (gotresponse->array[cur_proc].burstTime * 1.0);
            WTA[count_done] = curWTA;
            count_done += 1;

            totalWait += waiting_time;
            totalWTA += curWTA;

            expectedTime += gotresponse->array[cur_proc].burstTime;

            // Clean all about this process
            swap(&gotresponse->array[cur_proc], &gotresponse->array[gotresponse->head]);
            pop(gotresponse);
            cur_proc = -1;
        }

        int best_in_v = -1;
        // Getting the process with least rem time in main queue
        for (int i = v->head; i <= v->tail; i++)
        {
            if (v->array[i].arrivalTime <= getClk())
            {
                // If this process is better than last chosen process ==> best_in_v = i
                if (best_in_v == -1 || v->array[i].remainingTime < v->array[best_in_v].remainingTime)
                    best_in_v = i;
            }
        }

        if (cur_proc != -1)
        {
            // there is a process running in current time

            if (best_in_v != -1 && gotresponse->array[cur_proc].remainingTime > v->array[best_in_v].remainingTime)
            {
                // A new comming process has less shorter rem time than the currently working one

                // Allocating memory
                pair memPosition = allocate(&freeArr, memoryArr, v->array[best_in_v].memorySize);
                if (memPosition.start == -1 && memPosition.end == -1)
                {
                    // Move this process away and continue working on the already running process
                    swap(&v->array[best_in_v], &v->array[v->tail]);
                }
                else
                {
                    //Update process info
                    v->array[best_in_v].memoryStartIndex = memPosition.start;
                    v->array[best_in_v].memoryEndIndex = memPosition.end;

                    int memSize = memPosition.end - memPosition.start + 1;
                    fprintf(memory_log, "At time %d allocatd %d bytes for process %d from %d to %d\n", getClk(), memSize, v->array[best_in_v].ID, memPosition.start, memPosition.end);

                    /* Sigstop to the current process  */
                    gotresponse->array[cur_proc].lastRunTime = getClk();

                    action_log(file, &gotresponse->array[cur_proc], "stopped", getClk());

                    push(gotresponse, v->array[best_in_v]);
                    swap(&v->array[v->head], &v->array[best_in_v]);
                    pop(v);
                    cur_proc = gotresponse->tail;

                    gotresponse->array[cur_proc].pid = create_new_process(&gotresponse->array[cur_proc]);

                    action_log(file, &gotresponse->array[cur_proc], "started", getClk());
                }
            }
        }
        else
        {
            // There is no process running this time

            // Looking for the best process to be in action from the processes those worked in past
            int best_in_gotresponse = -1;
            for (int i = gotresponse->head; i <= gotresponse->tail; i++)
            {
                if (best_in_gotresponse == -1 || gotresponse->array[i].remainingTime < gotresponse->array[best_in_gotresponse].remainingTime)
                    best_in_gotresponse = i;
            }
            bool v_nxt = 0;
            if (best_in_gotresponse != -1 && best_in_v != -1)
            {
                // Can be either a new process or a already worked process

                if (gotresponse->array[best_in_gotresponse].remainingTime < v->array[best_in_v].remainingTime)
                    v_nxt = 0;

                else
                    v_nxt = 1;
            }
            else if (best_in_gotresponse != -1 && best_in_v == -1)
                v_nxt = 0;

            else if (best_in_gotresponse == -1 && best_in_v != -1)
                v_nxt = 1;
            else
            {
                // No current working process, no new process and no got-response process will work
                while (getClk() == last_clk)
                {
                }
                continue;
            }

            if (v_nxt)
            {
                // A new process will take over

                // Get a piece of memory
                PCB p = v->array[best_in_v];
                pair memPosition = allocate(&freeArr, memoryArr, v->array[best_in_v].memorySize);
                int memSize = memPosition.end - memPosition.start + 1;
                fprintf(memory_log, "At time %d allocatd %d bytes for process %d from %d to %d\n", getClk(), memSize, p.ID, memPosition.start, memPosition.end);

                // Pushing this new process into the got-response queue, it's no more a new process.
                push(gotresponse, v->array[best_in_v]);
                swap(&v->array[v->head], &v->array[best_in_v]);
                pop(v);

                cur_proc = gotresponse->tail;

                gotresponse->array[cur_proc].pid = create_new_process(&gotresponse->array[cur_proc]);
                action_log(file, &gotresponse->array[cur_proc], "started", getClk());
            }
            else
            {
                // A got-response process will take over
                cur_proc = best_in_gotresponse;
                /* This process that will go into work, is already created, then just wake it up */
                action_log(file, &gotresponse->array[cur_proc], "resumed", getClk());
            }
        }

        kill(gotresponse->array[cur_proc].pid, SIGCONT);

        // If reaches here, then there is a running process.
        msg.mtype = gotresponse->array[cur_proc].pid;
        //printf("Parent sending to procss id: %ld\n", msg.mtype);
        msgsnd(msgq_id1, &msg, sizeof(msg.mtext), !IPC_NOWAIT);

        msg.mtype = getpid();
        //printf("Parent is waiting\n");
        msgrcv(msgq_id2, &msg, sizeof(msg.mtext), msg.mtype, !IPC_NOWAIT);
        //printf("Parent got from procss id: %d\n", gotresponse->array[cur_proc].pid);

        kill(gotresponse->array[cur_proc].pid, SIGSTOP);

        gotresponse->array[cur_proc].remainingTime--;
    }

    // calculating perf file
    trueTime = getClk() - first_clk;

    float avgWTA = totalWTA / n;
    float avgWait = totalWait / n;
    float cpuUtilize = (expectedTime / trueTime) * 100;
    float stdWTA = 0;
    for (int i = 0; i < n; i++)
    {
        stdWTA += (WTA[i] - avgWTA) * (WTA[i] - avgWTA);
    }
    stdWTA /= n;
    stdWTA = sqrt(stdWTA);
    ///////////////////////
    fprintf(scheduler_perf, "CPU utilization = %.2f%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nstd WTA = %.2f", cpuUtilize, avgWTA, avgWait, stdWTA);
    fclose(scheduler_perf);

    fclose(file);

    // printf("Done\n");
}