#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <errno.h>
#define BUF_SIZE 3
#define SHM_BUFF 101
#define SHM_INDX_PRODUCED 103
#define SHM_INDX_CONSUMED 105
#define SEM_BIN_KEY 107
#define SEM_RES_KEY 113
#define SEM_FULL_KEY 115
#define SEM_EMPTY_KEY 117

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};
/**
 * chekc if shared memory for producer and consume r index exist or not to decide if it will be initialized
*/
int initSharedMemoryBuff()
{
    int shmid = shmget(SHM_BUFF, sizeof(int) * BUF_SIZE, 0666 | IPC_CREAT);

    return shmid;
}

int initSharedMemoryIndexProduced()
{
    int shmid = shmget(SHM_INDX_PRODUCED, sizeof(int), 0666 | (IPC_CREAT | IPC_EXCL));
    if (shmid == -1)
    {
        shmid = shmget(SHM_INDX_PRODUCED, sizeof(int), 0666 | (IPC_CREAT));
    }
    else
    {
        int *shmaddr = (int *)shmat(shmid, (void *)0, 0);
        *shmaddr = 0;
        shmdt(shmaddr);
    }

    return shmid;
}

int initSharedMemoryIndexConsumed()
{
    int shmid = shmget(SHM_INDX_CONSUMED, sizeof(int), 0666 | (IPC_CREAT | IPC_EXCL));
    if (shmid == -1)
    {
        shmid = shmget(SHM_INDX_CONSUMED, sizeof(int), 0666 | (IPC_CREAT));
    }
    else
    {
        int *shmaddr = (int *)shmat(shmid, (void *)0, 0);
        *shmaddr = 0;
        shmdt(shmaddr);
    }

    return shmid;
}
/**
 * check if first time semaphore is created if first time then intialise its value 
 * else return id 
*/

int initMutexSem()
{
    int semid = semget(SEM_BIN_KEY, 1, 0666 | (IPC_CREAT | IPC_EXCL));
    if (semid == -1)
    {
        semid = semget(SEM_BIN_KEY, 1, 0666 | IPC_CREAT);
    }
    else
    {
        union Semun semun;
        semun.val = 1;
        semctl(semid, 0, SETVAL, semun);
    }
    return semid;
}

int initFullSem()
{
    int semid = semget(SEM_FULL_KEY, BUF_SIZE, 0666 | (IPC_CREAT | IPC_EXCL));
    if (semid == -1)
    {
        semid = semget(SEM_FULL_KEY, 1, 0666 | IPC_CREAT);
    }
    else
    {
        union Semun semun;
        semun.val = 0;
        semctl(semid, 0, SETVAL, semun);
    }
    return semid;
}

int initEmptySem()
{
    int semid = semget(SEM_EMPTY_KEY, BUF_SIZE, 0666 | (IPC_CREAT | IPC_EXCL));
    if (semid == -1)
    {
        semid = semget(SEM_EMPTY_KEY, 1, 0666 | IPC_CREAT);
    }
    else
    {
        union Semun semun;
        semun.val = BUF_SIZE;
        semctl(semid, 0, SETVAL, semun);
    }
    return semid;
}

void down(int semid)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(semid, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int semid)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = 1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(semid, &p_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}
// add item to buffer and increase full value
void produceItem(int buffId, int producedIndxId, int *x)
{
    int *buffer = (int *)shmat(buffId, (void *)0, 0);
    int *producedIndexAddr = (int *)shmat(producedIndxId, (void *)0, 0);
    int currAddr = (*producedIndexAddr) % BUF_SIZE;
    (*producedIndexAddr)++;
    buffer[currAddr] = *producedIndexAddr;
    *x = *producedIndexAddr;
    shmdt(buffer);
    shmdt(producedIndexAddr);
}
// consume item on buffer and update value
void consumeItem(int buffId, int consumedIndxId, int *x)
{
    int *buffer = (int *)shmat(buffId, (void *)0, 0);
    int *consumedIndexAddr = (int *)shmat(consumedIndxId, (void *)0, 0);
    int currAddr = (*consumedIndexAddr) % BUF_SIZE;
    (*consumedIndexAddr)++;
    buffer[currAddr] = 0;
    *x = *consumedIndexAddr;
    shmdt(buffer);
    shmdt(consumedIndexAddr);
}
void printBuffer(int buffId)
{
    int *buffer = (int *)shmat(buffId, (void *)0, 0);
    for (int i = 0; i < BUF_SIZE; i++)
    {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}
void clearIPCResources()
{
    int semFullId = initFullSem();
    int semEmptyId = initEmptySem();
    int semMutexId = initMutexSem();

    int buffId = initSharedMemoryBuff();
    int produceIndexId = initSharedMemoryIndexProduced();
    int consumedIndexId = initSharedMemoryIndexConsumed();

    // clear IPCs
    shmctl(buffId, IPC_RMID, (struct shmid_ds *)0);
    shmctl(produceIndexId, IPC_RMID, (struct shmid_ds *)0);
    shmctl(consumedIndexId, IPC_RMID, (struct shmid_ds *)0);

    semctl(semFullId, 0, IPC_RMID);
    semctl(semMutexId, 0, IPC_RMID);
    semctl(semEmptyId, 0, IPC_RMID);
}