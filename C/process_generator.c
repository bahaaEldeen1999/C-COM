#include "headers/headers.h"
#include "headers/file_handler.h"
#include "headers/buddy_algorithm.h"
#include <math.h>


pid_t scheduler_pid, clock_pid;
void clearResources(int);
void initialize_shm_buddy(int);
void writer(int, vector *);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    int shmk_buddy = ftok("key_buddy", 10);
    int shmid_buddy = shmget(shmk_buddy, 1024, IPC_CREAT | 0666);
    initialize_shm_buddy(shmid_buddy);

    int shmk = ftok("key_process_table", 65);
    int shmid = shmget(shmk, 4096, IPC_CREAT | 0666);
    vector processTable = fileHandler();
    writer(shmid, &processTable);
    // printf("elm0.id=%d\n", get(&processTable, 0).ID);

    char algorithmNumber[2], quantum[256];
    printf("Enter 0 for RR\nEnter 1 for SRTN\nEnter 2 for HPF\n");
    scanf("%c", algorithmNumber);
    if (algorithmNumber[0] == '0')
    {
        printf("Enter value for quantum\n");
        scanf("%s", quantum);
    }
    else
    {
        strcpy(quantum, "-1");
    }

    scheduler_pid = fork();
    if (scheduler_pid == 0)
    {
        char *args[] = {algorithmNumber, quantum};
        printf("in child 1: %s %s\n", algorithmNumber, quantum);
        execlp("./scheduler.out", "./scheduler.out", algorithmNumber, quantum, NULL);
        //exit(0);
    }

    clock_pid = fork();
    if (clock_pid == 0)
    {
        printf("in child 2\n");
        execlp("./clk.out", "./clk.out", NULL);

        //exit(0);
    }
    initClk();
    int x = getClk();
    printf("current time is %d\n", x);

    destroyClk(false);
}

void clearResources(int signum)
{
    kill(scheduler_pid, SIGKILL);
    kill(clock_pid, SIGKILL);
}

void writer(int shmid, vector *processTable)
{
    vector *shmaddr = (vector *)shmat(shmid, (void *)0, 0);
    initialize((vector *)shmaddr, size(processTable));
    // //memcpy(shmaddr, processTable, sizeof(processTable));
    for (int i = 0; i < size(processTable); i++)
    {
        ((vector *)shmaddr)->array[i] = processTable->array[i];
        ((vector *)shmaddr)->head = processTable->head;
        ((vector *)shmaddr)->tail = processTable->tail;
    }

    printVector((shmaddr));
    shmdt(shmaddr);
}

void initialize_shm_buddy(int shmid_buddy)
{
    void *shmaddr_buddy = shmat(shmid_buddy, (void *)0, 0);
    char arr[1024];
    for (int i = 0; i < 1024; i++)
    {
        arr[i] = '0';
    }
    strcpy((char *)shmaddr_buddy, arr);
    printf("%c\n", ((char *)shmaddr_buddy)[10]);
    shmdt(shmaddr);

    initialize_shm(shmid_buddy);
}