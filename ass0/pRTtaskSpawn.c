#include <vxworks.h>
#include <tasklib.h>
#include "pRTtaskspawn.h"

int pRTtaskSpawn(char *name, int priority, int options, int stacksize,
                 int period, int WECT,
                 int a, ..., int j) 
{
    // ...
    for (i = 0; i < n; i++) // creating tasks uniquely with their names and ids taken into consideration
    {
        // ...
        ID[i] = taskSpawn(...);
    }

    // ...

    while (...) 
    {
        // suspends a task x ticks
        taskSuspend(ID[i], x);
        // adds x ticks to execution time of this task
        totalSuspend[i] += x;

        // ...

        // if all tasks are executed successfully in a period it starts executing for the next period
        if (i++ > n)
        {
            taskDelay(period); // calling the taskDelay() to delay the period int j;
            for (j = 0; j < n; j++) // sets the incoming task priority to High priority
            {
                // ...
            }
            i = 0; // pointing to the start of the array
        }
    }

    for (i = 0; i < n; i++) // if all tasks complete execution then the memory is freed
    {
        // ...
    }

    return taskIdSelf(); // returns the taskID for the current executing task
}
