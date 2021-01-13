#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "headers.h"
#include <unistd.h>
int main()
{
    int semFullId = initFullSem();
    int semEmptyId = initEmptySem();
    int semMutexId = initMutexSem();

    int buffId = initSharedMemoryBuff();
    int produceIndexId = initSharedMemoryIndexProduced();
    int consumedIndexId = initSharedMemoryIndexConsumed();

    printf("mutexID %d\nemptyID %d\nfullID %d\n", semMutexId, semEmptyId, semFullId);
    while (1)
    {
        down(semFullId);
        down(semMutexId);
        int x;
        consumeItem(buffId, consumedIndexId, &x);
        printf("consuming %d....\n", x);
        sleep(10);
        printf("consumed\n");
        up(semMutexId);
        up(semEmptyId);
    }
}