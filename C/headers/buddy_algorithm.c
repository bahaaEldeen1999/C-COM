#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_SIZE 512

struct pair{

    int start;
    int end;

} pair;

struct freeSizeList{

    struct pair pairList[MAX_SIZE];
    int lastIndex;

}freeSizeList;

struct freeList{

    struct freeSizeList free[10];

}freeList;

char memoryArr[1024];
struct freeList freeArr;

// ----------------------- Initialize the free list of size 1024 with the all memory size------------------------
void initializeFreeList() {
    freeArr.free[9].pairList[freeArr.free[9].lastIndex].start=0;
    freeArr.free[9].pairList[freeArr.free[9].lastIndex].end=1023;
    freeArr.free[9].lastIndex +=1;
}

// -------------------------------- Allocate a memory for a processes with fixed size----------------------------
struct pair allocate(int requiredSize) {

    //Initilaize the pair with (-1,-1) (not found answer)
    struct pair result = {.start = -1, .end = -1};

    int n = ceil(log(requiredSize) / log(2))-1;


    if(freeArr.free[n].lastIndex > 0) {

        // Store start & end index of the memory
        result.start = freeArr.free[n].pairList[0].start;
        result.end = freeArr.free[n].pairList[0].end;

        // Delete this block from the free memory
        freeArr.free[n].pairList[0] = freeArr.free[n].pairList[freeArr.free[n].lastIndex];
        freeArr.free[n].lastIndex -=1;

    } else {

        int i;
        // Search for bigger available block
        for (i=n+1; i<10; i++ ) {
            if(freeArr.free[i].lastIndex != 0)
                break;
        }

        // Cannot allocate memory
        if (i == 10 ) {
            printf("cannot allocate memory\n");
            return result;
        }

        // Save the result
        result.start = freeArr.free[i].pairList[0].start;
        result.end = freeArr.free[i].pairList[0].end;

        // Delete this block from the free memory
        freeArr.free[i].pairList[0] = freeArr.free[i].pairList[freeArr.free[i].lastIndex-1];
        freeArr.free[i].lastIndex -=1;

        // Divide blocks into small ones if possible

        // The new blocks will be of size = i-1
        i-=1;
        while(i >= n) {

            // Construct new two blocks with smaller size
            struct pair pair1,pair2;
            pair1.start = result.start;
            pair1.end = result.start+ (result.end - result.start)/2;

            pair2.start = result.start+ (result.end - result.start +1)/2;
            pair2.end = result.end;

            // Add the blocks to the array
            freeArr.free[i].pairList[freeArr.free[i].lastIndex] = pair1;
            freeArr.free[i].lastIndex +=1;

            freeArr.free[i].pairList[freeArr.free[i].lastIndex] = pair2;
            freeArr.free[i].lastIndex +=1;

            // for(int j =0;j<freeArr.free[i].lastIndex;j++) {
            //     printf("%d %d \n", freeArr.free[i].pairList[j].start,freeArr.free[i].pairList[j].end);
            // }


            // Save the result
            result.start = freeArr.free[i].pairList[0].start;
            result.end = freeArr.free[i].pairList[0].end;


            // Remove the first block with size i after allocate it
            freeArr.free[i].pairList[0] = freeArr.free[i].pairList[freeArr.free[i].lastIndex-1];
            freeArr.free[i].lastIndex -=1;

            // for(int j =0;j<freeArr.free[i].lastIndex;j++) {
            //     printf("%d %d \n", freeArr.free[i].pairList[j].start,freeArr.free[i].pairList[j].end);
            // }


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

// -------------------------------- Free process memory------------------------------------
void deallocate(int startIndex, int endIndex) {

    int blockSize = endIndex - startIndex +1;
    int n = ceil(log(blockSize)/log(2))-1;

    struct pair addedBlock = {.start = startIndex, .end = endIndex };

    freeArr.free[n].pairList[freeArr.free[n].lastIndex] = addedBlock;
    freeArr.free[n].lastIndex +=1;

    for(int i = startIndex; i<=endIndex;i++) {
        memoryArr[i] = '0';
    }
    printf("Memory freeed from %d to %d \n", startIndex,endIndex);
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
    for(int i=0;i<freeArr.free[n].lastIndex;i++) {

        // Buddy is found
        if(freeArr.free[n].pairList[i].start == buddyStartAddress) {
            //printf("buddy found at %d  me at %d\n" , i,freeArr.free[n].lastIndex);
            // Calculate the new block start & end depending on buddy position
            struct pair newPair;
            if(buddyNum %2 == 0) {

                newPair.start = startIndex;
                newPair.end = startIndex + 2 * blockSize -1;

                // printf("buddy left to me\n");
                // printf("new pair %d %d \n", newPair.start, newPair.end);

            } else {

                newPair.start = buddyStartAddress;
                newPair.end = buddyStartAddress + 2*blockSize;

            }

            // printf("n+1 before \n");
            // for(int j =0;j<freeArr.free[n+1].lastIndex;j++) {
            //     printf("%d %d \n", freeArr.free[n+1].pairList[j].start,freeArr.free[n+1].pairList[j].end);
            // }
            // Store the new block on the free list of size n+1
            freeArr.free[n+1].pairList[freeArr.free[n+1].lastIndex] = newPair;
            freeArr.free[n+1].lastIndex +=1;


            // printf("n+1 after \n");
            // for(int j =0;j<freeArr.free[n+1].lastIndex;j++) {
            // printf("%d %d \n", freeArr.free[n+1].pairList[j].start,freeArr.free[n+1].pairList[j].end);
            // }
        }

        // printf("n before \n");
        // for(int j =0;j<freeArr.free[n].lastIndex;j++) {
        //     printf("%d %d \n", freeArr.free[n].pairList[j].start,freeArr.free[n].pairList[j].end);
        // }

        // Remove the 2 blocks
        freeArr.free[n].lastIndex -=1;

        freeArr.free[n].pairList[i] = freeArr.free[n].pairList[freeArr.free[n].lastIndex-1];
        freeArr.free[n].lastIndex -=1;

        // printf("n after \n");
        // for(int j =0;j<freeArr.free[n].lastIndex;j++) {
        //     printf("%d %d \n", freeArr.free[n].pairList[j].start,freeArr.free[n].pairList[j].end);
        // }
        break;
    }

}

int main() {
    initializeFreeList();
    struct pair temp =allocate(16);
    deallocate(temp.start,temp.end);
    temp = allocate(16);
    // temp =allocate(64);
    // temp =allocate(16);
}