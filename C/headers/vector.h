#pragma once
#include "pcb.h"

#define MAX_SIZE 2048

typedef struct
{
      __int32_t tail, head;
      PCB array[MAX_SIZE];
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
      PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U', .pid = 0, .waitTime = 0, .remainingTime = 0, .index = 0};
      for (__int32_t i = v->head; i <= v->tail; i++)
      {
            equalize(&v->array[i], &x);
      }

      v->head = 0;
      v->tail = sz - 1;
}

// ----------------------------------- array ONLY usage functions ----------------------------------

PCB find(vector *v, __uint32_t id)
{
      for (__int32_t i = v->head; i <= v->tail; i++)
      {
            if (v->array[i].ID == id)
                  return v->array[i];
      }
      PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U', .pid = 0, .waitTime = 0, .remainingTime = 0, .index = 0};
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
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U', .pid = 0, .waitTime = 0, .remainingTime = 0, .index = 0};
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
      equalize(&v->array[index], &item);
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
            initialize(v, 0);
      }
}

PCB top(vector *v)
{
      if (v->head > v->tail)
      {
            printf("access violation: queue is empty!\n");
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U', .pid = 0, .waitTime = 0, .remainingTime = 0, .index = 0};
            return x;
      }
      return v->array[v->head];
}

void push(vector *v, PCB item)
{
      if (v->tail + 1 >= MAX_SIZE)
      {
            printf("queue has reached maximum allowable memory, please re initialize to be able to push again\n");
      }
      v->tail++;
      set(v, v->tail, item);
}
