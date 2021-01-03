#include "headers/headers.h"

int main(int argc, char *argv[])
{
    initClk();

    //TODO implement the scheduler :)
    printf("scheduler is running and got params: %s %s\n", argv[0], argv[1]);
    //upon termination release the clock resources.

    destroyClk(true);
}
