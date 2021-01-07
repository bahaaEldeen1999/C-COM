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
void roundRobin(vector *process, unsigned int quantum, int msgqid)
{
    //printf("pid %d\n", getpid());
    vector q;
    initialize(&q, 0);
    //push(&q, get(process, 0));
    int time = getClk(); //get(&q, 0).arrivalTime; // this is gonna change to globl clock
    //integer_queue remaining_time;
    //initializeQueue(&remaining_time, 0);
    //pushQueue(&remaining_time, get(&q, 0).burstTime);
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
                //pushQueue(&remaining_time, get(process, i).burstTime);
                curr_index++;
            }
            else
            {
                break;
            }
        }
        //printf("size Q %d\n", size(&q));
        if (size(&q) == 0)
        {
            //time += 1;
            continue;
        }
        //printf("h1\n");
        PCB p = top(&q);
        pop(&q);
        //printf("h2\n");
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
            //printf("last run time %d wait time %d\n", p.lastRunTime, p.waitTime);
            p.waitTime += time - p.lastRunTime;
            printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime);
        }
        if (rem_time > quantum)
        {

            while (getClk() - time < quantum)
            {
                //printf("rem time %d\n", rem_time);

                // check if first time then fork the process and run it for d=first time
                if (firstTime == 1)
                {

                    firstTime = 0;
                    int pid = fork();
                    if (pid == 0)
                    {
                        //printf("start process %d for first time \n", p.ID);
                        // execute the process.o
                        char rem[100];
                        sprintf(rem, "%d", p.remainingTime);
                        execlp("./process.out", "./process.out", rem, NULL);
                    }
                    else
                    {
                        // set process pid to pid
                        p.pid = pid;
                        msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
                        kill(p.pid, SIGSTOP);
                        //raise(SIGSTOP);
                    }
                }
                else
                {
                    // resume process
                    //printf("cont 1 %d\n", p.pid);
                    kill(p.pid, SIGCONT);
                    msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
                    //printf("stp 1\n");
                    kill(p.pid, SIGSTOP);
                    // stop myself
                    //raise(SIGSTOP);
                }
                // stop prcess as soon as time finish
                //kill(p.pid, SIGSTOP);
            }
            rem_time -= quantum;
            time += quantum;
        }
        else
        {
            while (getClk() - time < rem_time)
            {
                //printf("rem time in sched %d\n", rem_time);
                //printf("clock %d time %d \n", getClk(), time);
                // check if first time then fork the process and run it for d=first time
                if (firstTime == 1)
                {
                    firstTime = 0;
                    int pid = fork();
                    if (pid == 0)
                    {
                        char cwd[1000];
                        //printf("start process %d for first time \n", p.ID);
                        // execute the process.o
                        // getcwd(cwd, 1000);
                        // strcat(cwd, "/process.out");
                        // printf("%s\n", cwd);
                        char rem[100];
                        sprintf(rem, "%d", p.remainingTime);
                        int x = execl("./process.out", "./process.out", rem, NULL);
                        //printf("err %d\n", x);
                    }
                    else
                    {
                        // set process pid to pid
                        p.pid = pid;
                        //printf("cild pid %d\n", p.pid);
                        msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
                        //printf("stopped\n");
                        //printf("cild pid %d\n", p.pid);
                        kill(p.pid, SIGSTOP);
                        //raise(SIGSTOP);
                    }
                }
                else
                {
                    // resume process
                    //kill(p.pid, SIGCONT);
                    // stop myself
                    //raise(SIGSTOP);
                    //printf("cont 2 %d\n", p.pid);
                    kill(p.pid, SIGCONT);
                    msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
                    //printf("stp 2\n");
                    kill(p.pid, SIGSTOP);
                }
                // stop prcess as soon as i wakeup
                //kill(p.pid, SIGSTOP);
            }

            time += rem_time;
            rem_time = 0;
            kill(p.pid, SIGCONT);
        }

        //printf("rem time %d\n", rem_time);

        if (rem_time > 0)
        {

            p.lastRunTime = time;
            p.remainingTime = rem_time;
            push(&q, p);
            //printf("h3\n");
            //pushQueue(&remaining_time, rem_time);
            printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime);
        }
        else
        {
            printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, p.ID, p.arrivalTime, p.burstTime, rem_time, p.waitTime, time - p.arrivalTime, (1.0 * (time - p.arrivalTime)) / (float)(1.0 * p.burstTime));
        }
        //printf("sizeQ %d\n", size(&q));
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