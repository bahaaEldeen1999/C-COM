#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "headers.h"
#include <unistd.h>
/**
 * initlaize shared memory if not already initializerd 
 * get size of buffer from a header file 
 * add semaphores counting semaphores for shared resource 
 * maybe we need to add binary semaphotres for mutex lock
 **/

int main()
{
    int semFullId = initFullSem();
    int semEmptyId = initEmptySem();
    int semMutexId = initMutexSem();

    int buffId = initSharedMemoryBuff();
    int produceIndexId = initSharedMemoryIndexProduced();
    int consumedIndexId = initSharedMemoryIndexConsumed();

    // printf("mutexID %d\nemptyID %d\nfullID %d\n", semFullId, semEmptyId, semMutexId);
    while (1)
    {
        // check there is space to add more items
        down(semEmptyId);
        // obtain the lock
        down(semMutexId);
        int x;
        produceItem(buffId, produceIndexId, &x);
        //printf("producing %d...\n", x);
        sleep(2);
        printf("producer: inserted %d\n", x);
        //printBuffer(buffId);
        fflush(stdout);
        // remove lock
        up(semMutexId);
        // add item produced
        up(semFullId);
    }
}
