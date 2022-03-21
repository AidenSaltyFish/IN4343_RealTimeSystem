#include <stdio.h>
#include <vxworks.h>
#include <tasklib.h>
#include <string.h>
#include "pRTtaskspawn.h"

#define n 20 // number of tasks=20
#define WECT 20
#define highpriority 10
#define lowpriority 30
#define size_task 15

int pRTtaskSpawn(char *name, int priority, int options, int stacksize,
                 int period, int WECT,
                 int a, int b, int c, int d, int e, int f,
                 int g1, int h, int i, int j) 
int g = 0; // A flag variable for keeping a check on number of tasks; initially 0
{
    int i;
    char **tname = malloc(n * sizeof(char *)); // allocating memory for tasks where arrays of names and ids of every task is stored
    int *ID = malloc(n * sizeof(int));
    for (i = 0; i < n; i++) // creating tasks uniquely with their names and ids taken into consideration
    {
        *tname[i] = malloc(size_task * sizeof(char));
        sprintf(tname[i], "Task%d", i);
        ID[i] = taskSpawn(tname[i], highpriority, 0, 10000, (FUNCPTR)task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    i = 0; // pointing to the beginning of the array
    int totalSuspend[n];
    // sets it to zeros
    memset(totalSuspend, 0, n * sizeof(int));
    while (g < 20) // while tasks(20) execution
    {
        // suspends a task 5 ticks
        taskSuspend(ID[i], 5);
        // adds 5 ticks to execution time of this task
        totalSuspend[i] += 5;
        // if this task is overrunning, then sets its priority low
        if (totalSuspend[i] > WCET)
        {
            // if the priority of a task is high, set it to low
            if (taskGetPriority(ids[i], highpriority))
            {
                taskSetPriority(ids[i], lowpriority);
            }
        }
        // if all tasks are executed successfully in a period it starts executing for the next period
        if (i++ > n)
        {
            taskDelay(period); // calling the taskDelay() to delay the period int j;
            for (j = 0; j < n; j++) // sets the incoming task priority to High priority
            {
                taskSetPriority(ID[i], highpriority);
                taskSuspend[i] = 0;
            }
            i = 0; // pointing to the start of the array
        }
    }
    for (i = 0; i < n; i++) // if all tasks complete execution then the memory is freed
    {
        free(tname[i]);
    }
    free(tname);
    return taskIdSelf(); // returns the taskID for the current executing task
}
