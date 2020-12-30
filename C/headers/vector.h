#include "pcb.h"

typedef struct
{
      __uint32_t size_;
      __int32_t tail, head;
      PCB *array;
} vector;

// -------------------------------------Helper Functions---------------------------------------------

void initialize(vector *v, __uint32_t sz)
{
      v->head = 0;
      v->tail = sz - 1;
      v->array = malloc((sz + 10) * sizeof(PCB));
      v->size_ = sz + 10;
}

__int32_t size(vector *v)
{
      return v->tail + 1;
}

PCB get(vector *v, __int32_t index)
{
      if (index < v->head || index > v->tail)
      {
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
            return x;
      }
      return v->array[index];
}

void set(vector *v, __int32_t index, PCB item)
{
      if (index < v->head || index > v->tail)
            return;
      v->array[index].arrivalTime = item.arrivalTime;
      v->array[index].burstTime = item.burstTime;
      v->array[index].startTime = item.startTime;
      v->array[index].finishTime = item.finishTime;
      v->array[index].lastRunTime = item.lastRunTime;
      v->array[index].ID = item.ID;
      v->array[index].priority = item.priority;
      v->array[index].state = item.state;
}

void pop(vector *v)
{
      if (v->head > v->tail)
            return;

      v->head++;
}

PCB top(vector *v)
{
      if (v->head > v->tail)
      {
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
            return x;
      }
      return v->array[v->head];
}

void push(vector *v, PCB item)
{
      if (v->tail == v->size_)
      {
            vector *tmp;
            tmp->array = malloc((v->size_ + 1000) * sizeof(PCB));
            for (__int32_t i = v->head; i < v->tail; i++)
            {
                  set(tmp, i, v->array[i]);
            }
            tmp->tail = v->tail + 1, tmp->size_ = v->size_ + 1000, tmp->head = v->head;
            v = tmp;
      }
      else
      {
            v->tail++;
      }

      set(v, v->tail, item);
}

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
