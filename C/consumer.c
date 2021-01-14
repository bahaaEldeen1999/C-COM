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

    //printf("mutexID %d\nemptyID %d\nfullID %d\n", semMutexId, semEmptyId, semFullId);
    while (1)
    {
        // check that there is indeed item produced
        down(semFullId);
        // mutual exclusion
        down(semMutexId);
        int x;
        consumeItem(buffId, consumedIndexId, &x);
        //printf("consuming %d....\n", x);
        sleep(1);
        printf("Consume value %d\n", x);
        //printBuffer(buffId);
        fflush(stdout);
        // release the lock
        up(semMutexId);
        // increase empty area
        up(semEmptyId);
    }
}