#ifndef __Schedule_c
#define __Schedule_c

#include "Clock.h"
#include "Scheduler.h"
#include "Led.h"
#include "Context.h"

Task Tasks[NUMTASKS];           /* Lower indices: lower priorities           */
uint16_t NextInterruptTime;     /* Timestamp at which next interrupt should occur */

uint16_t IntDisable (void)
{
  uint16_t sw;
    // sw = r2
  asm volatile ("mov.w r2, %0\n\t" : "=r"(sw));
  _DINT();
  return (sw);
}

void RestoreSW (uint16_t sw)
{
    // r2 = sw
  asm volatile ("mov.w %0, r2\n\t" :: "r"(sw));
}  

/* 
 * Initialize and clear task structures.
 * Should be called with interrupt disabled.
 * The clock must be started elsewhere.
 */

void InitTasks (void)
{			
  uint8_t i=NUMTASKS-1; 
  do { 
    Taskp t = &Tasks[i];
    t->Flags = t->Activated = t->Invoked = 0;
  } while (i--);
}

/*
 * Register a task, TRIGGERED only, with flags.
 * Testing and filling in defaults is done.
 * Each priority level has at most one task.
 */

uint8_t RegisterTask (uint16_t Phasing, uint16_t Period, uint16_t ExecutionTime,
                      void (*TaskFunc) (uint16_t), uint8_t Prio, uint8_t Flags)
{
  uint8_t  rtc = E_SUCCESS;
  uint16_t sw;

  Phasing *= 4; // correct to get ms approximately
  Period *=4; // correct to get ms approximately

  if (Prio>=NUMTASKS) return (E_BOUNDS); // out of bounds
  if (Period == 0) return (E_WRONGPAR);
  sw = IntDisable (); 
  Taskp t = &Tasks[Prio]; 
  if (t->Flags) rtc = E_BUSY; 
  else {
    t->NextRelease = 0 + Phasing;
    t->Period    = Period; 
    t->NextPendingDeadline = t->NextRelease + Period;
    t->Activated = t->Invoked = 0; 
    t->Taskf     = TaskFunc; 
    t->ExecutionTime = ExecutionTime;
    t->Flags     = Flags | TRIGGERED;

  }
  RestoreSW (sw);
  return (rtc);
}

uint8_t UnRegisterTask (uint8_t t)
{
  Tasks[t].Flags = 0;
  return (E_SUCCESS);
}  

static void DetermineNextInterruptTime (CandidateValue)
{
  if (CandidateValue < NextInterruptTime)
  {
    NextInterruptTime = CandidateValue;
  }
}

/*
  The timer interrupt is configured in continious mode with compare mode interrupts enabled.
  The source for Timer_A is ACLK/8 (32768 / 8 = 4096 Hz). Thus every 1/4096 second, the TAR register is incremented.
  When the TAR register reaches TACCR0, an interrupt is generated. Note that TAR will NOT be reset!
  It continues incrementing until it reaches it's max value (16-bit timer -> 65535), it will continue counting from 0.

  Relevant registers:
    TAR     Current value of Timer_A. 
    TACCR0  Compare register, when TAR reached TACCR0, an interrupt is generated

  Your job is to set a new value of TACCR0 every timer interrupt, to get the next interrupt

  Use the Task data structure defined in 'Scheduler.h' to store information which can be used by the scheduler.

*/

interrupt (TIMERA0_VECTOR) TimerIntrpt (void)
{
  ContextSwitch();

  /* ----------------------- INSERT CODE HERE ----------------------- */

  /* Insert timer interrupt logic, what tasks are pending? */ 
  /* When should the next timer interrupt occur? Note: we only want interrupts at job releases */

  /* Super simple, single task example */
  // Taskp t = &Tasks[0];
  // t->NextRelease += t->Period; // set next release time
  // t->Activated++;
  // NextInterruptTime = t->NextRelease;
  /* End of example*/


  uint8_t i;
  // whether_avtive to record whether there is a job is active
  uint8_t whether_active=NUMTASKS;
  // newly_activated to record newly activate job
  uint8_t newly_activated=NUMTASKS;

  //when interrupt judge whether new request is coming
  for (i=0;i<NUMTASKS;i++)
  {
    Taskp t=&Tasks[i];
    if (t->Flags & TRIGGERED){
      if ((t->NextRelease==NextInterruptTime)){
        SetDebug(0,1);
        t->Activated++; 
        newly_activated=(newly_activated<i)?newly_activated:i;
        // Pending |= i < BusyPrio;
        SetDebug(0,0);
      }
    }
    if (t->Flags & ACTIVE)
    {
      whether_active=(whether_active<i)?whether_active:i;
    }
  }

  //if the most prior newly activated one is higher prior than the task executing, then interrupt
  if (newly_activated<whether_active)
  {
    whether_active=NUMTASKS;
  }

  //find next time interrupt
  uint16_t NextReleaseTime = 0xFFFF;

  for(i=0;i<NUMTASKS;i++)
  {
    Taskp t = &Tasks[i];
    if (t->Flags & TRIGGERED) { 

      SetDebug(2,1);

      // always try to find the minimum one
      if (NextReleaseTime>t->NextRelease + t->Period)
      {
        SetDebug(3,1);
        NextReleaseTime= t->NextRelease + t->Period;
        SetDebug(3,0);
      }

      SetDebug(2,0);

    }
  }

  for (i=0;i<NUMTASKS;i++)
  {
    Taskp t = &Tasks[i];
    if (NextReleaseTime==0xFFFF)
    {
      SetDebug(4,1);
      break;
    }
    if ((t->Flags & TRIGGERED) && (NextReleaseTime == t->NextRelease + t->Period)) 
    {
      SetDebug(5,1);
      t->NextRelease = NextReleaseTime;
    }
    SetDebug(5,0);
  }

  NextInterruptTime = NextReleaseTime;

  TACCR0 = NextInterruptTime;

  if (whether_active==NUMTASKS){
    CALL_SCHEDULER;
  }

  // if (Pending)
  // {

  //   Scheduler_P_FP(Tasks);
  //   SetDebug(6,0);
  //   Pending=0;
  // }
  


  /* ---------------------------------------------------------------- */
 

  // TACCR0 = NextInterruptTime;

  // CALL_SCHEDULER;

  ResumeContext();
}

#endif

