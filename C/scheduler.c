#include "headers/headers.h"
#include "headers/round_robin.h"
#include "headers/highest_priority_first.h"
#include "headers/message_buffer.h"
#include <sys/shm.h>
int main(int argc, char *argv[])
{
    initClk();

    int shmid, pid;

    //printf("scheduler pid %d\n", getpid());
    //roundRobin((vector *)NULL, 0);
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    key_t key_id;
    int msgq_id1, msgq_id2, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id1 = msgget(key_id, 0666 | IPC_CREAT);
    msgq_id2 = msgget(key_id + 6, 0666 | IPC_CREAT);
    // shmid = shmget(key_id, 4096, IPC_CREAT | 0644);

    // // get PCB
    // vector *PCB = getPCB(shmid);

    // int algorithmNumber = atoi(argv[1]);
    // switch (algorithmNumber)
    // {
    // case 0:
    //     // RR

    //     break;
    // case 1:
    //     // SRTN
    //     break;
    // case 2:
    //     // HPF
    //     break;

    // default:
    //     break;
    // }
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
    HPF(&process, msgq_id1, msgq_id2);
    destroyClk(true);
}

void *getPCB(int shmid)
{
    void *shmaddr = shmat(shmid, (void *)0, 0);
    return shmaddr;
}