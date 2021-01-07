#include "headers/headers.h"
#include "headers/round_robin.h"
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
    int msgq_id, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    shmid = shmget(key_id, 4096, IPC_CREAT | 0644);

    // get PCB
    vector *PCB = getPCB(shmid);

    int algorithmNumber = atoi(argv[1]);
    switch (algorithmNumber)
    {
    case 0:
        // RR

        break;
    case 1:
        // SRTN
        break;
    case 2:
        // HPF
        break;

    default:
        break;
    }
    // vector process;
    // printf("initialze process vector\n");
    // initialize(&process, 0);
    // PCB p;
    // p.waitTime = 0;
    // p.ID = 0;
    // p.arrivalTime = 0;
    // p.burstTime = 2;
    // p.startTime = -1;
    // p.remainingTime = p.burstTime;
    // push(&process, p);
    // p.ID = 1;
    // p.arrivalTime = 0;
    // p.burstTime = 4;
    // p.startTime = -1;
    // p.remainingTime = p.burstTime;
    // push(&process, p);
    // p.ID = 2;
    // p.arrivalTime = 1;
    // p.burstTime = 5;
    // p.startTime = -1;
    // p.remainingTime = p.burstTime;
    // push(&process, p);
    // p.ID = 3;
    // p.arrivalTime = 2;
    // p.burstTime = 9;
    // p.startTime = -1;
    // p.remainingTime = p.burstTime;
    // push(&process, p);
    // p.ID = 4;
    // p.arrivalTime = 8;
    // p.burstTime = 7;
    // p.startTime = -1;
    // p.remainingTime = p.burstTime;
    // push(&process, p);
    // printf("round robin\n");
    // roundRobin(&process, 2, msgq_id);
    destroyClk(true);
}

void *getPCB(int shmid)
{
    void *shmaddr = shmat(shmid, (void *)0, 0);
    return shmaddr;
}