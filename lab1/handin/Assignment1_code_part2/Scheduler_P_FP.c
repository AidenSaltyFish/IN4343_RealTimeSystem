#include "Scheduler.h"
#include "Led.h"
#include "TimeTracking.h"

static void ExecuteTask (Taskp t)
{
  /* ----------------------- INSERT CODE HERE ----------------------- */

  t->Flags |= ACTIVE;

  t->Invoked++;
  t->Taskf(t->ExecutionTime); // execute task

  t->Flags &= (~ACTIVE);

  /* ---------------------------------------------------------------- */

}

void Scheduler_P_FP (Task Tasks[])
{ 
  /* ----------------------- INSERT CODE HERE ----------------------- */
  
  StartTracking(TT_SCHEDULER);

  SetLeds (BROWN, 1);

  uint8_t i;
  uint8_t prevPrio; 

  for (i = 0; i < NUMTASKS; i++) {
    Taskp CurTask = &(Tasks[i]);

    if (CurTask->Flags & ACTIVE) break;
  }

  prevPrio = i;

  for (i = 0; i < prevPrio; i++) {
    Taskp CurTask = &(Tasks[i]);

    while (CurTask->Activated != CurTask->Invoked) {
      if (CurTask->Flags & TRIGGERED) {
        SetLeds (BROWN, 0);
        StopTracking(TT_SCHEDULER);
        StopTracking(TT_TIMER_INTERRUPT);

        _EINT();
        ExecuteTask(CurTask); 
        _DINT();

        StartTracking(TT_SCHEDULER);
        StartTracking(TT_TIMER_INTERRUPT);
        SetLeds (BROWN, 1);
      } else {
        CurTask->Activated = CurTask->Invoked;
      }
    }
  }

  SetLeds (BROWN, 0);

  StopTracking(TT_SCHEDULER);
  
  /* ---------------------------------------------------------------- */
}
