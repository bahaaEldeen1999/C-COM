#include "headers/headers.h"
#include "headers/vector.h"
#include "headers/round_robin.h"
#include "headers/message_buffer.h"
#include "headers/shortest_remaining_time_next.h"
#include "headers/highest_priority_first.h"
#include <sys/shm.h>
vector *getPCB(int shmid);

void clearIPC(int msgqid1, int msgqid2, int shmid1);
int main(int argc, char *argv[])
{
    initClk();

    int shmk = ftok("key_process_table", 65);
    int shmid = shmget(shmk, 4096, IPC_CREAT | 0666);

    key_t key_id;
    int msgq_id1, msgq_id2, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id1 = msgget(key_id, 0666 | IPC_CREAT);
    msgq_id2 = msgget(key_id + 6, 0666 | IPC_CREAT);

    // get PCB
    int algorithmNumber = atoi(argv[1]);
    int quatum = atoi(argv[2]);
    vector *processTable = getPCB(shmid);

    switch (algorithmNumber)
    {
    case 0:
        // RR
        roundRobin(processTable, quatum, msgq_id1, msgq_id2);
        break;
    case 1:
        // SRTN
        SRTN(processTable, msgq_id1, msgq_id2);
        break;
    case 2:
        // HPF
        HPF(processTable, msgq_id1, msgq_id2);
        break;

    default:
        break;
    }
    clearIPC(msgq_id1, msgq_id2, shmid);
    destroyClk(true);
    return 0;
}

vector *getPCB(int shmid)
{

    vector *shmaddr = (vector *)shmat(shmid, (void *)0, 0);
    return shmaddr;
}

void clearIPC(int msgqid1, int msgqid2, int shmid1)
{
    msgctl(msgqid1, IPC_RMID, NULL);
    msgctl(msgqid2, IPC_RMID, NULL);
    shmctl(shmid1, IPC_RMID, NULL);
    printf("REMOVED IPC\n");
}