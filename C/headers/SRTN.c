#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "headers.h"

void action_log(FILE *file, PCB *item, char *statement, int clk)
{
    int waiting_time = clk + item->remainingTime - (item->arrivalTime + item->burstTime);

    if (statement == "finished")
    {
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %lf\n",
                clk, item->ID, statement, item->arrivalTime, item->burstTime,
                item->remainingTime, waiting_time, item->burstTime + waiting_time,
                ((double)item->burstTime + waiting_time) / (item->burstTime));
    }
    else
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                clk, item->ID, statement, item->arrivalTime, item->burstTime, item->remainingTime, waiting_time);
}

void SRTN(vector *v, int n)
{
    FILE *file = fopen("scheduler.log", "w");
    int clk;

    vector *gotresponse = malloc(0);
    initialize(gotresponse, 0);
    int cur_proc = -1;

    while (size(v) || size(gotresponse))
    {
        clk = getClk();
        printf("%d\n", clk);
        sleep(1);

        // See if there is currently running process and it finished. Then write in log, pop it from the queue, cur_proc=-1
        if (cur_proc != -1 && gotresponse->array[cur_proc].remainingTime == 0)
        {
            // Done Process
            action_log(file, &gotresponse->array[cur_proc], "finished", getClk());
            swap(&gotresponse->array[cur_proc], &gotresponse->array[gotresponse->head]);
            pop(gotresponse);
            cur_proc = -1;
        }

        int best_in_v = -1;
        // Getting the process with least rem time in main queue
        for (int i = v->head; i <= v->tail; i++)
        {
            if (v->array[i].arrivalTime <= clk)
            {
                // If this process is better than last chosen process ==> best_in_v = i
                if (best_in_v == -1 || v->array[i].remainingTime < v->array[best_in_v].remainingTime)
                    best_in_v = i;
            }
        }

        if (cur_proc != -1)
        {

            if (best_in_v != -1 && gotresponse->array[cur_proc].remainingTime > v->array[best_in_v].remainingTime)
            {
                action_log(file, &gotresponse->array[cur_proc], "stopped", getClk());
                /*gotresponse->array[cur_proc].state = "stopped";*/

                push(gotresponse, v->array[best_in_v]);
                swap(&v->array[v->head], &v->array[best_in_v]);
                pop(v);
                cur_proc = gotresponse->tail;
                action_log(file, &gotresponse->array[cur_proc], "started", getClk());
                /*gotresponse->array[cur_proc].state = "started";*/
            }
        }
        else
        {
            int best_in_gotresponse = -1;
            for (int i = gotresponse->head; i <= gotresponse->tail; i++)
            {
                if (best_in_gotresponse == -1 || gotresponse->array[i].remainingTime < gotresponse->array[best_in_gotresponse].remainingTime)
                    best_in_gotresponse = i;
            }
            bool v_nxt = 0;
            if (best_in_gotresponse != -1 && best_in_v != -1)
            {
                if (gotresponse->array[best_in_gotresponse].remainingTime < v->array[best_in_v].remainingTime)
                    v_nxt = 0;

                else
                    v_nxt = 1;
            }
            else if (best_in_gotresponse != -1 && best_in_v == -1)
                v_nxt = 0;

            else if (best_in_gotresponse == -1 && best_in_v != -1)
                v_nxt = 1;
            else
                continue;

            if (v_nxt)
            {
                push(gotresponse, v->array[best_in_v]);
                swap(&v->array[v->head], &v->array[best_in_v]);
                pop(v);

                cur_proc = gotresponse->tail;
                action_log(file, &gotresponse->array[cur_proc], "started", getClk());
                /*gotresponse->array[cur_proc].state = "started";*/
            }
            else
            {
                cur_proc = best_in_gotresponse;
                action_log(file, &gotresponse->array[cur_proc], "resumed", getClk());
                /*gotresponse->array[cur_proc].state = "resumed";*/
            }
        }

        // If reaches here, then there is a running process.
        gotresponse->array[cur_proc].remainingTime--;
    }
    fclose(file);
}

int main()
{
    initClk();

    vector v;
    initialize(&v, 0);

    PCB x = {.arrivalTime = 1, .remainingTime = 7, .burstTime = 7, .finishTime = 0, .ID = 1, .lastRunTime = 0, .priority = 5, .startTime = 0, .state = 'U'};
    PCB y = {.arrivalTime = 3, .remainingTime = 3, .burstTime = 3, .finishTime = 0, .ID = 2, .lastRunTime = 0, .priority = 3, .startTime = 0, .state = 'U'};
    PCB z = {.arrivalTime = 2, .remainingTime = 5, .burstTime = 5, .finishTime = 0, .ID = 3, .lastRunTime = 0, .priority = 3, .startTime = 0, .state = 'U'};
    push(&v, x);
    push(&v, y);
    push(&v, z);

    SRTN(&v, size(&v));

    destroyClk(1);
    return 0;
}