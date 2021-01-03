#include "headers/headers.h"
#include <stdio.h>
#include <stdlib.h>

int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    remainingtime = atoi(argv[1]);
    while (remainingtime > 0)
    {
        // sleep scheduler
        int lastTime = getClk();
        while (getClk() - lastTime < 1)
        {
        }
        lastTime = getClk();
        remainingtime -= 1;
        // wake scheduler
    }

    destroyClk(false);

    return 0;
}
