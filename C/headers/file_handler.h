#include "vector.h"

void testVectorValidity()
{
      vector v;
      initialize(&v, 3);
      printf("created vector successfully, testing it\n");
      unsigned int sz = size(&v);
      printf("testing size\nsize = %d\n", sz); // 3
      PCB x = {.arrivalTime = 1, .burstTime = 3, .finishTime = 7, .ID = 1, .lastRunTime = 10, .priority = 3, .startTime = 5, .state = 'S'};
      set(&v, 0, x);
      x = get(&v, 0);
      printf("testing set&get\nID=%d\n", x.ID); // 1
      x.ID = 4;
      set(&v, 0, x);
      x = top(&v);
      printf("testing top\nID=%d\n", x.ID); // 4
      x.ID = 3;
      set(&v, 1, x);
      x.ID = 5;
      x.priority = 100;
      set(&v, 2, x);
      x.priority = 50;
      x.ID = 7;
      push(&v, x);
      sz = size(&v);
      printf("testing push\nsize = %d\n", sz); // 4
      PCB y = find(&v, 5);
      printf("testing find\npriority = %d\n", y.priority); // 100
      sort(&v, "pri");
      for (int i = 0; i <= v.last; i++)
      {
            printf("%d\n", v.array[i].priority);
      }
      puts("");
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
                  input.ID = a, input.arrivalTime = b, input.burstTime = c, input.priority = d;
                  set(&v, i, input);
            }
      }
      else
      {
            printf("cannot open the file\n");
      }
      return v;
}
