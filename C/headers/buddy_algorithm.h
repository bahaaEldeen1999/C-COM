#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "headers.h"
#include <string.h>
#include <math.h>

#define MAX_SIZE 512

typedef struct{

    int start;
    int end;

} pair;

typedef struct{

    pair pairList[MAX_SIZE];
    int lastIndex;

}freeSizeList;

typedef struct{

    freeSizeList free[10];

}freeList;

//extern struct freeList freeArr;

// ----------------------- Initialize the free list of size 1024 with the all memory size------------------------
void initializeFreeList( freeList *freeArr) {
    printf("from init \n");
    printf("%d \n",freeArr->free[9].lastIndex);
    freeArr->free[9].pairList[freeArr->free[9].lastIndex].start=0;
    freeArr->free[9].pairList[freeArr->free[9].lastIndex].end=1023;
    freeArr->free[9].lastIndex +=1;
}

// -------------------------------- Allocate a memory for a processes with fixed size----------------------------
pair allocate(freeList *freeArr,char *memoryArr,int requiredSize) {
printf("free %d \n", freeArr->free[9].lastIndex);
    //Initilaize the pair with (-1,-1) (not found answer)
    pair result = {.start = -1, .end = -1};

    if (requiredSize == 0) return result;

    int n = ceil(log(requiredSize) / log(2))-1;

    printf(" n %d \n",n);
    if(freeArr->free[n].lastIndex > 0) {

        // Store start & end index of the memory
        result.start = freeArr->free[n].pairList[0].start;
        result.end = freeArr->free[n].pairList[0].end;

        // Delete this block from the free memory
        freeArr->free[n].pairList[0] = freeArr->free[n].pairList[freeArr->free[n].lastIndex-1];
        freeArr->free[n].lastIndex -=1;

    } else {

        int i;
        // Search for bigger available block
        for (i=n+1; i<10; i++ ) {
            //printf("i %d %d \n",i,freeArr->free[i].lastIndex);
            if(freeArr->free[i].lastIndex != 0)
                break;
        }

        // Cannot allocate memory
        if (i == 10 ) {
            printf("cannot allocate memory\n");
            return result;
        }

        // Save the result
        result.start = freeArr->free[i].pairList[0].start;
        result.end = freeArr->free[i].pairList[0].end;

        // Delete this block from the free memory
        freeArr->free[i].pairList[0] = freeArr->free[i].pairList[freeArr->free[i].lastIndex-1];
        freeArr->free[i].lastIndex -=1;

        // Divide blocks into small ones if possible

        // The new blocks will be of size = i-1
        i-=1;
        while(i >= n) {

            // Construct new two blocks with smaller size
            pair pair1,pair2;
            pair1.start = result.start;
            pair1.end = result.start+ (result.end - result.start)/2;

            pair2.start = result.start+ (result.end - result.start +1)/2;
            pair2.end = result.end;

            // Add the blocks to the array
            freeArr->free[i].pairList[freeArr->free[i].lastIndex] = pair1;
            freeArr->free[i].lastIndex +=1;

            freeArr->free[i].pairList[freeArr->free[i].lastIndex] = pair2;
            freeArr->free[i].lastIndex +=1;

            // Save the result
            result.start = freeArr->free[i].pairList[0].start;
            result.end = freeArr->free[i].pairList[0].end;


            // Remove the first block with size i after allocate it
            freeArr->free[i].pairList[0] = freeArr->free[i].pairList[freeArr->free[i].lastIndex-1];
            freeArr->free[i].lastIndex -=1;

            i-=1;
        }
    }
    printf("Allocate memory start %d end %d for %d \n",result.start,result.end,requiredSize);

    // Update the memory array
    for(int j=result.start; j<= result.end; j++) {
        memoryArr[j] = '1';
    }
    return result;
}

// // -------------------------------- Merge any two buddies blocks----------------------------
void mergeBlocks(freeList *arr, int sizeIndex, int blockSize , int startIndex) {
    arr->free[2].lastIndex=2;

    if (sizeIndex >= 9) return;
    int n = sizeIndex;
    //printf("%d \n",arr->free[n].lastIndex);

    // Merge two blocks if possible
    int buddyNum = startIndex / blockSize;
    int buddyStartAddress;

    // Calculate the buddy address
    if(buddyNum %2 ==0) {

        //   --------------------------
        //   |Added block| Buddy block|
        //   --------------------------
        buddyStartAddress = startIndex + blockSize;
    } else {

        //   --------------------------
        //   |Buddy block| Added block|
        //   --------------------------
        buddyStartAddress = startIndex - blockSize;
    }
    // Search for the buddy
    //printf("%d \n",arr->free[n].lastIndex);
    for(int i=0;i<arr->free[n].lastIndex;i++) {
        // Buddy is found
        if(arr->free[n].pairList[i].start == buddyStartAddress) {
            // Calculate the new block start & end depearr->free[n].lastIndexnding on buddy position
            pair newPair;
            if(buddyNum %2 == 0) {

                newPair.start = startIndex;
                newPair.end = startIndex + 2 * blockSize -1;

            } else {

                newPair.start = buddyStartAddress;
                newPair.end = buddyStartAddress + 2*blockSize;

            }

            // Store the new block on the free list of size n+1
            arr->free[n+1].pairList[arr->free[n+1].lastIndex] = newPair;
            arr->free[n+1].lastIndex +=1;

            mergeBlocks(arr,n+1,blockSize*2,newPair.start);
            // Remove the 2 blocks
            arr->free[n].lastIndex -=1;

            arr->free[n].pairList[i] = arr->free[n].pairList[arr->free[n].lastIndex-1];
            arr->free[n].lastIndex -=1;
            break;
        }
    }
}

// // -------------------------------- Free process memory------------------------------------
void deallocate(freeList *freeArr,char *memoryArr,int startIndex, int endIndex) {

    int blockSize = endIndex - startIndex +1;
    int n = ceil(log(blockSize)/log(2))-1;
    pair addedBlock = {.start = startIndex, .end = endIndex };

    freeArr->free[n].pairList[freeArr->free[n].lastIndex] = addedBlock;
    freeArr->free[n].lastIndex +=1;

    for(int i = startIndex; i<=endIndex;i++) {
        memoryArr[i] = '0';
    }
    mergeBlocks(freeArr,n,blockSize,startIndex);

    printf("Memory freeed from %d to %d \n", startIndex,endIndex);

}
// // -------------------------------- Initilize the shared memory----------------------------
void initialize_shm(int shmid_buddy,char *memoryArr) {
    void *shmaddr_buddy = shmat(shmid_buddy, (void *)0, 0);
    strcpy(memoryArr,(char *)shmaddr_buddy);
}