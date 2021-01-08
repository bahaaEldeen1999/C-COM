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

void SRTN(vector *v, int msgq_id1, int msgq_id2)
{
    //printf("***** SRTN *****\n");

    int n = size(v);
    FILE *file = fopen(output_path, "w");
    FILE *scheduler_perf = fopen("./scheduler.perf", "w");
    fprintf(file, "#At time x process y state arr w total z remain y wait k \n");

    vector *gotresponse = (vector *)malloc(0);
    initialize(gotresponse, 0);
    int cur_proc = -1;

    struct msgbuff msg;
    //printf("sizeV %d \n", size(v));

    // calc perf
    float expectedTime = 0;
    float trueTime = 0;
    float totalWTA = 0;
    float totalWait = 0;
    int numOfProcess = size(v);

    //////////////////////////////
    float *WTA = (float *)malloc(sizeof(float) * (numOfProcess + 1));
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
            action_log(file, &gotresponse->array[cur_proc], "finished", getClk());
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
        else
        {
            int best_in_gotresponse = -1;
            for (int i = gotresponse->head; i <= gotresponse->tail; i++)
            {
                if (best_in_gotresponse == -1 || gotresponse->array[i].remainingTime < gotresponse->array[best_in_gotresponse].remainingTime)
                    best_in_gotresponse = i;
            }
            bool v_nxt = 0;
            if (best_in_gotresponse != -1 && best_in_v != -1)
            {
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
                while (getClk() == last_clk)
                {
                }
                continue;
            }

            if (v_nxt)
            {

                push(gotresponse, v->array[best_in_v]);
                swap(&v->array[v->head], &v->array[best_in_v]);
                pop(v);

                cur_proc = gotresponse->tail;

                gotresponse->array[cur_proc].pid = create_new_process(&gotresponse->array[cur_proc]);
                action_log(file, &gotresponse->array[cur_proc], "started", getClk());
            }
            else
            {
                cur_proc = best_in_gotresponse;

                /* This process that will go into work, is already created, then just wake it up. */

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
    float avgWTA = totalWTA / numOfProcess;
    float avgWait = totalWait / numOfProcess;
    float cpuUtilize = (expectedTime / trueTime) * 100;
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

    fclose(file);

    // printf("Done\n");
}