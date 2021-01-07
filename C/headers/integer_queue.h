#pragma once

typedef struct
{

    int *array;
    __int32_t tail, head;
} integer_queue;

/*Helper Functions*/

// ----------------------- general usage (by both queue and array) functions------------------------

__int32_t sizeQueue(integer_queue *v)
{
    return (v->tail - v->head + 1);
}

void initializeQueue(integer_queue *v, __uint32_t sz)
{
    v->head = 0;
    v->tail = sz - 1;
    v->array = malloc((sz) * sizeof(int));
}

void setQueue(integer_queue *v, __int32_t index, int item)
{
    if (index < v->head || index > v->tail)
    {
        printf("access violation: index out of range!\n");
        return;
    }
    v->array[index] = item;
}

// ----------------------------------- array ONLY usage functions ----------------------------------

// ----------------------------------- queue ONLY usage functions ----------------------------------

void popQueue(integer_queue *v)
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
        initializeQueue(v, 0);
    }
}

int topQueue(integer_queue *v)
{
    if (v->head > v->tail)
    {
        printf("access violation: queue is empty!\n");

        return 0;
    }
    return v->array[v->head];
}

void pushQueue(integer_queue *v, int item)
{
    if (v->tail == -1 || v->tail % 10 == 9)
    {
        integer_queue tmp;
        initializeQueue(&tmp, 11 + v->tail);
        for (__int32_t i = v->head; i <= v->tail; i++)
            setQueue(&tmp, i, v->array[i]);
        v->array = tmp.array;
    }
    v->tail++;
    setQueue(v, v->tail, item);
}