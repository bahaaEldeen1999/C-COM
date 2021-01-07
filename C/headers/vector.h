#pragma once
#include "pcb.h"

typedef struct
{
      __int32_t tail, head;
      PCB *array;
} vector;

/*Helper Functions*/

// ----------------------- general usage (by both queue and array) functions------------------------

__int32_t size(vector *v)
{
      return (v->tail - v->head + 1);
}

void printVector(vector *v)
{
      for (int i = v->head; i <= v->tail; i++)
      {
            printPCB(v->array[i]);
      }
}

void initialize(vector *v, __uint32_t sz)
{
      v->head = 0;
      v->tail = sz - 1;
      v->array = malloc((sz) * sizeof(PCB));
}

// ----------------------------------- array ONLY usage functions ----------------------------------

PCB find(vector *v, __uint32_t id)
{
      for (__int32_t i = v->head; i <= v->tail; i++)
      {
            if (v->array[i].ID == id)
                  return v->array[i];
      }
      PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
      return x;
}

void sort(vector *v, char sortingKey[])
{
      for (__int32_t i = v->head; i <= v->tail; i++)
      {
            for (__int32_t j = i + 1; j <= v->tail; j++)
            {
                  if (compare(&v->array[i], &v->array[j], sortingKey) > 0)
                        swap(&v->array[i], &v->array[j]);
            }
      }
}

PCB get(vector *v, __int32_t index)
{
      if (index < v->head || index > v->tail)
      {
            printf("access violation: index out of range!\n");
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
            return x;
      }
      return v->array[index];
}

void set(vector *v, __int32_t index, PCB item)
{
      if (index < v->head || index > v->tail)
      {
            printf("access violation: index out of range!\n");
            return;
      }
      v->array[index].arrivalTime = item.arrivalTime;
      v->array[index].burstTime = item.burstTime;
      v->array[index].startTime = item.startTime;
      v->array[index].finishTime = item.finishTime;
      v->array[index].lastRunTime = item.lastRunTime;
      v->array[index].remainingTime = item.remainingTime;
      v->array[index].waitTime = item.waitTime;
      v->array[index].pid = item.pid;
      v->array[index].ID = item.ID;
      v->array[index].priority = item.priority;
      v->array[index].state = item.state;
      v->array[index].pindx = item.pindx;
}

// ----------------------------------- queue ONLY usage functions ----------------------------------

void pop(vector *v)
{
      if (v->head > v->tail)
      {
            printf("access violation: queue already is empty!\n");
            return;
      }

      v->head++;

      if (v->head > v->tail)
      {
            // re-initialize the array to avoid memory leak
            initialize(v, 0);
      }
}

PCB top(vector *v)
{
      if (v->head > v->tail)
      {
            printf("access violation: queue is empty!\n");
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
            return x;
      }
      return v->array[v->head];
}

void push(vector *v, PCB item)
{
      if (v->tail == -1 || v->tail % 10 == 9)
      {
            vector tmp;
            initialize(&tmp, 11 + v->tail);
            for (__int32_t i = v->head; i <= v->tail; i++)
                  set(&tmp, i, v->array[i]);
            v->array = tmp.array;
      }
      v->tail++;
      set(v, v->tail, item);
}
