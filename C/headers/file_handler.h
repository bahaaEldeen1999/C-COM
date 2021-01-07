#pragma once
#include "vector.h"

void testVectorValidity()
{
      PCB x = {.arrivalTime = 0, .burstTime = 0, .finishTime = 0, .lastRunTime = 0, .startTime = 0, .state = 'N'};
      //--------------------------- act as a fixed sized array
      printf("------------------ testing array functionalities ------------------\n");
      vector array;
      initialize(&array, 5);
      for (int i = 0; i < 5; i++)
      {
            x.ID = i;
            x.priority = 4 - i;
            set(&array, i, x);
      }
      for (int i = 0; i < 5; i++)
      {
            x = get(&array, i);
            printf("array[%d].ID=%d  should be=%d\n", i, x.ID, i);
      }

      x = find(&array, 3);
      printf("find element: ID=%d  should be=3\n", x.ID);

      sort(&array, "priority");
      printVector(&array);

      //--------------------------- act as a queue
      printf("------------------ testing queue functionalities ------------------\n");
      vector queue;
      initialize(&queue, 0);
      for (int i = 0; i < 5; i++)
      {
            x.ID = i;
            x.priority = 4 - i;
            push(&queue, x);
      }

      printf("size=%d  should be=5\n", size(&queue));

      x = top(&queue);
      printf("top.ID=%d  should be=0\n", x.ID);

      for (int i = 0; i < 5; i++)
      {
            pop(&queue);
      }

      printf("size=%d  should be=0\n", size(&queue));
}

vector fileHandler(int numberOfProcesses)
{
      FILE *file = fopen("processes.txt", "r");
      vector v;
      if (file)
      {
            char line[256];
            int a, b, c, d;
            PCB input;
            fgets(line, sizeof(line), file);
            initialize(&v, numberOfProcesses);
            for (__int32_t i = 0; i < numberOfProcesses; i++)
            {
                  fscanf(file, "%d\t%d\t%d\t%d", &a, &b, &c, &d);
                  input.ID = a, input.arrivalTime = b, input.burstTime = c, input.priority = d, input.state = 'N', input.remainingTime = c, input.startTime = 0, input.finishTime = 0, input.lastRunTime = 0, input.waitTime = 0, input.startTime = -1;
                  set(&v, i, input);
            }
      }
      else
      {
            printf("cannot open the file\n");
      }
      return v;
}