#include "headers/headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include "headers/message_buffer.h"
int remainingtime;

int main(int agrc, char *argv[])
{

    initClk();
    //printf("start process\n");
    key_t key_id;
    int msgq_id, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    struct msgbuff message;
    message.mtype = getpid();
    strcpy(message.mtext, "stop");
    remainingtime = atoi(argv[1]);
    while (remainingtime > 0)
    {
        int lastTime = getClk();
        while (getClk() - lastTime <= 1)
        {
        }
        lastTime = getClk();
        remainingtime -= 1;
        msgsnd(msgq_id, &message, sizeof(message.mtext), !IPC_NOWAIT);
    }
    destroyClk(false);

    return 0;
}
