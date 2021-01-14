#pragma once
#include "vector.h"
#include <string.h>

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

vector fileHandler()
{
      FILE *file = fopen("processes.txt", "r");
      vector v;
      if (file)
      {
            char line[256];
            PCB input;
            initialize(&v, 0);

            fgets(line, sizeof(line), file);
            while (fgets(line, sizeof(line), file) != NULL)
            {
                  if (line[0] == '#')
                        continue;

                  __uint32_t numbers[5], i, iterator = 0, numberIndex = 0;
                  char *number = malloc(10);
                  for (i = 0; i < strlen(line); i++)
                  {
                        if (line[i] != '\t' && line[i] != '\n')
                        {
                              number[iterator++] = line[i];
                        }
                        else
                        {
                              numbers[numberIndex++] = atoi(number);
                              iterator = 0;
                              number = malloc(10);
                        }
                  }
                  input.ID = numbers[0], input.arrivalTime = numbers[1], input.burstTime = numbers[2], input.priority = numbers[3],
                  input.state = 'N', input.remainingTime = numbers[2], input.startTime = 0, input.finishTime = 0, input.lastRunTime = 0,
                  input.waitTime = 0, input.startTime = -1, input.memorySize = number[4],
                  input.memoryStartIndex = 0, input.memoryEndIndex = 0;
                  push(&v, input);
                  memset(line, '\0', sizeof(line));
            }
      }
      else
      {
            printf("cannot open the file\n");
      }
      return v;
}