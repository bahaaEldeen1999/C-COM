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
#include "message_buffer.h"
void roundRobin(vector *process, unsigned int quantum, int msgqid1, int msgqid2)
{

    vector q;
    initialize(&q, 0);
    int time = getClk();
    struct msgbuff message;
    int curr_index = 0;
    while (size(&q) > 0 || curr_index < size(process))
    {
        time = getClk();
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
            firstTime = 1;
            p.waitTime = time - p.arrivalTime;
            p.startTime = time;
            printf("At time %d process %d started arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, p.burstTime, p.waitTime);
        }
        else
        {
            p.waitTime += time - p.lastRunTime;
            printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime);
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
            rem_time -= quantum;
            time += quantum;
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
                    kill(p.pid, SIGCONT);
                    message.mtype = p.pid;
                    msgsnd(msgqid1, &message, sizeof(message.mtext), !IPC_NOWAIT);
                    message.mtype = getpid();
                    msgrcv(msgqid2, &message, sizeof(message.mtext), message.mtype, !IPC_NOWAIT);
                    message.mtype = getpid();
                    kill(p.pid, SIGSTOP);
                }
            }

            time += rem_time;
            rem_time = 0;
            int state;
            waitpid(p.pid, &state, (int)NULL);
        }

        if (rem_time > 0)
        {

            p.lastRunTime = time;
            p.remainingTime = rem_time;
            push(&q, p);
            printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime);
        }
        else
        {
            printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime, time - p.arrivalTime, (1.0 * (time - p.arrivalTime)) / (float)(1.0 * p.burstTime));
        }
    }
}

/*
int main()
{
    initClk();
    vector process;
    printf("initialze process vector\n");
    initialize(&process, 0);
    PCB p;
    p.waitTime = 0;
    p.ID = 0;
    p.arrivalTime = 0;
    p.burstTime = 2;
    p.startTime = -1;
    p.remainingTime = p.burstTime;
    push(&process, p);
    p.ID = 1;
    p.arrivalTime = 0;
    p.burstTime = 4;
    p.startTime = -1;
    p.remainingTime = p.burstTime;
    push(&process, p);
    p.ID = 2;
    p.arrivalTime = 1;
    p.burstTime = 5;
    p.startTime = -1;
    p.remainingTime = p.burstTime;
    push(&process, p);
    p.ID = 3;
    p.arrivalTime = 2;
    p.burstTime = 9;
    p.startTime = -1;
    p.remainingTime = p.burstTime;
    push(&process, p);
    p.ID = 4;
    p.arrivalTime = 8;
    p.burstTime = 7;
    p.startTime = -1;
    p.remainingTime = p.burstTime;
    push(&process, p);
    printf("round robin\n");
    roundRobin(&process, 2);
    return 0;
}
*/