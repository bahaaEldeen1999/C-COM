#include "vector.h"

void testVectorValidity()
{
      vector v;
      initialize(&v, 3);
      printf("created vector successfully, testing it\n");

      __uint32_t sz = size(&v);
      printf("testing size, size(should be 3)=%d\n", sz); // 3

      PCB x = {.arrivalTime = 1, .burstTime = 3, .finishTime = 7, .ID = 1, .lastRunTime = 10, .priority = 3, .startTime = 5, .state = 'N'};
      set(&v, 0, x);
      x = get(&v, 0);
      printf("testing set&get, ID(should be 1)=%d\n", x.ID); // 1

      x.ID = 4;
      set(&v, 0, x);
      x = top(&v);
      printf("testing top, ID(should be 4)=%d\n", x.ID); // 4

      x.ID = 5;
      x.priority = 400;
      set(&v, 1, x);
      pop(&v);
      x = top(&v);
      printf("testing pop, ID(should be 5)=%d\n", x.ID); // 5

      x.ID = 6;
      x.priority = 100;
      set(&v, 2, x);
      x.priority = 50;
      x.ID = 7;
      push(&v, x);
      sz = size(&v);
      printf("testing push, size(should be 4)=%d\n", sz); // 4

      x = find(&v, 5);
      printf("testing find, priority(should be 400)=%d\n", x.priority); // 400

      printf("testing sorting\n");
      sort(&v, "pri");
      for (int i = v.head; i <= v.tail; i++)
      {
            printf("priority=%d\tID=%d\n", v.array[i].priority, v.array[i].ID);
      }
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
                  input.ID = a, input.arrivalTime = b, input.burstTime = c, input.priority = d, input.state = 'N';
                  set(&v, i, input);
            }
      }
      else
      {
            printf("cannot open the file\n");
      }
      return v;
}
