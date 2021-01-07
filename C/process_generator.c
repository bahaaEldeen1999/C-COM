#include "headers/headers.h"
#include "headers/file_handler.h"

pid_t scheduler_pid, clock_pid;
void clearResources(int);

int main(int argc, char *argv[])
{
    testVectorValidity();
    // signal(SIGINT, clearResources);

    // vector processTable = fileHandler(atoi(argv[1]));
    // printf("elm0.id=%d\n", get(&processTable, 0).ID);

    // char algorithmNumber[2], quantum[256];
    // printf("Enter 0 for RR\nEnter 1 for SRTN\nEnter 2 for HPF\n");
    // scanf("%c", algorithmNumber);
    // if (algorithmNumber[0] == '0')
    // {
    //     printf("Enter value for quantum\n");
    //     scanf("%s", quantum);
    // }
    // else
    // {
    //     strcpy(quantum, "-1");
    // }

    // scheduler_pid = fork();
    // if (scheduler_pid == 0)
    // {
    //     char *args[] = {algorithmNumber, quantum};
    //     printf("in child 1: %s %s\n", algorithmNumber, quantum);
    //     execv("scheduler.out", args);
    //     exit(0);
    // }

    // clock_pid = fork();
    // if (clock_pid == 0)
    // {
    //     printf("in child 2\n");
    //     execv("clk.out", (char *[]){});
    //     exit(0);
    // }

    // initClk();
    // destroyClk(false);
}

void clearResources(int signum)
{
    kill(scheduler_pid, SIGKILL);
    kill(clock_pid, SIGKILL);
}
