#include "headers/headers.h"
#include "headers/vector.h"
#include "headers/SRTN.h"
#include "headers/message_buffer.h"
#include <sys/shm.h>
vector *getPCB(int shmid);
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

    printf("h1\n");
    vector *processTable = getPCB(shmid);
    
    switch (algorithmNumber)
    {
    case 0:
        // RR
        break;
    case 1:
        // SRTN
        SRTN(processTable, msgq_id1, msgq_id2);
        break;
    case 2:
        // HPF
        break;

    default:
        break;
    }
    printf("Scheduler is Done.\n");
    destroyClk(true);
}

vector *getPCB(int shmid)
{
    vector *shmaddr = (vector *)shmat(shmid, (void *)0, 0);
    return shmaddr;
}