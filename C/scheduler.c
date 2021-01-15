#include "headers/headers.h"
#include "headers/vector.h"
#include "headers/round_robin.h"
#include "headers/message_buffer.h"
#include "headers/shortest_remaining_time_next.h"
#include "headers/highest_priority_first.h"
#include <sys/shm.h>
vector *getPCB(int shmid);

char *getSharedMemory(int shmid);
int main(int argc, char *argv[])
{
    initClk();
    printf("run sc\n");

    int shmk = ftok("key_process_table", 65);
    int shmid = shmget(shmk, 4096, IPC_CREAT | 0666);
    printf("shmid %d\n", shmid);

    key_t key_id;
    int msgq_id1, msgq_id2, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id1 = msgget(key_id, 0666 | IPC_CREAT);
    msgq_id2 = msgget(key_id + 6, 0666 | IPC_CREAT);
    // shmid = shmget(key_id, 4096, IPC_CREAT | 0644);

    // get PCB
    int algorithmNumber = atoi(argv[1]);
    int quatum = atoi(argv[2]);
    int shmid_buddy = atoi(argv[3]);

    // get shared memory
    char *shmidArr = getSharedMemory(shmid_buddy);
    //printf("h1\n");
    vector *processTable = getPCB(shmid);

    switch (algorithmNumber)
    {
    case 0:
        // RR
        roundRobin(processTable, quatum, msgq_id1, msgq_id2, shmidArr);
        break;
    case 1:
        // SRTN
        SRTN(processTable, msgq_id1, msgq_id2, shmidArr);
        break;
    case 2:
        // HPF
        HPF(processTable, msgq_id1, msgq_id2, shmidArr);
        break;

    default:
        break;
    }

    destroyClk(true);
}

vector *getPCB(int shmid)
{

    vector *shmaddr = (vector *)shmat(shmid, (void *)0, 0);
    return shmaddr;
}

char *getSharedMemory(int shmid)
{
    char *memAddr = (char *)shmat(shmid, (void *)0, 0);
    return memAddr;
}