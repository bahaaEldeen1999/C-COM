#include "pcb.h"

typedef struct
{
      __uint8_t isSorted;
      __uint32_t size_;
      __int32_t last;
      PCB *array;
} vector;

// -------------------------------------Helper Functions---------------------------------------------

void initialize(vector *v, __uint32_t sz)
{
      v->isSorted = 0;
      v->last = sz - 1;
      v->array = malloc((sz + 10) * sizeof(PCB));
      v->size_ = sz + 10;
}

__int32_t size(vector *v)
{
      return v->last + 1;
}

PCB get(vector *v, __int32_t index)
{
      if (index < 0 || index > v->last)
      {
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
            return x;
      }
      return v->array[index];
}

void set(vector *v, __int32_t index, PCB item)
{
      if (index < 0 || index > v->last)
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
      if (v->last < 0)
            return;
      v->array[v->last].arrivalTime = 0, v->array[v->last].burstTime = 0, v->array[v->last].startTime = 0,
      v->array[v->last].finishTime = 0, v->array[v->last].lastRunTime = 0, v->array[v->last].ID = 0, v->array[v->last].priority = 0, v->array[v->last].state = 'U';
      v->last--;
}

PCB top(vector *v)
{
      if (v->last < 0)
      {
            PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
            return x;
      }
      return v->array[0];
}

void push(vector *v, PCB item)
{
      if (v->last == v->size_)
      {
            vector *tmp;
            tmp->array = malloc((v->size_ + 1000) * sizeof(PCB));
            for (__uint32_t i = 0; i < v->last; i++)
            {
                  set(tmp, i, v->array[i]);
            }
            tmp->last = v->last + 1, tmp->size_ = v->size_ + 1000;
            v = tmp;
      }
      else
      {
            v->last++;
      }
      set(v, v->last, item);
}

PCB find(vector *v, __uint32_t id)
{
      for (__uint32_t i = 0; i <= v->last; i++)
      {
            if (v->array[i].ID == id)
                  return v->array[i];
      }
      PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .ID = 0, .lastRunTime = 0, .priority = 0, .startTime = 0, .state = 'U'};
      return x;
}

void sort(vector *v, char sortingKey[])
{
      for (__int32_t i = 0; i <= v->last; i++)
      {
            for (__int32_t j = i + 1; j <= v->last; j++)
            {
                  if (compare(&v->array[i], &v->array[j], sortingKey) > 0)
                        swap(&v->array[i], &v->array[j]);
            }
      }
}
