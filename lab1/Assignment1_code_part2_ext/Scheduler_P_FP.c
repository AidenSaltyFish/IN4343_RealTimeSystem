#include "Scheduler.h"
#include "Led.h"


static void ExecuteTask (Taskp t)
{
  /* ----------------------- INSERT CODE HERE ----------------------- */

  t->Flags =t->Flags | ACTIVE;
  t->Invoked++;
  t->Taskf(t->ExecutionTime); // execute task
  t->Flags=t->Flags & (~ACTIVE);

  /* ---------------------------------------------------------------- */

}

void Scheduler_P_FP (Task Tasks[])
{ 
  /* ----------------------- INSERT CODE HERE ----------------------- */

  /* Super simple, single task example */
  // Taskp t = &Tasks[0];
  // if (t->Activated != t->Invoked)
  // {
  //   ExecuteTask(t);
  // }

  SetLeds (BROWN, 0);

  uint8_t i;

  // uint8_t oldBP = BusyPrio; // Set BusyPrio as the priority of the currently running task (the one that was executing just before the scheduler is called)

  for(i=0;i<NUMTASKS;i++)
  {
    Taskp CurTask = &Tasks[i];
    while (CurTask->Activated != CurTask->Invoked) {
      if (CurTask->Flags & TRIGGERED) {
        SetLeds (BROWN, 1);
        _EINT(); 
        ExecuteTask(CurTask);
        _DINT();
        SetLeds (BROWN, 0);
        // CALL_SCHEDULER;
      } else {
        CurTask->Activated = CurTask->Invoked;
      }
    }
  }

  SetLeds (BROWN, 1);
  /* End of example*/

  /* ---------------------------------------------------------------- */
}
