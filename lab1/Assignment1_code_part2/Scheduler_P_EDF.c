#include "Scheduler.h"
#include "Led.h"
#include "TimeTracking.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t ScheduleTimes[NUMTASKS] = { 0 };

/* ----------- doublely linked list ------------ */
struct node {
  uint8_t TaskIdx;
  uint16_t TaskNextPendingDeadline;
	
  struct node *next;
  struct node *prev;
};

//this link always point to first Link
struct node *head = NULL;

//this link always point to last Link 
struct node *tail = NULL;

void insertNode (struct node* curNode, uint8_t TaskIdx, uint16_t TaskNextPendingDeadline) {
  struct node *link = (struct node*) malloc(sizeof(struct node));
  link->TaskIdx = TaskIdx;
  link->TaskNextPendingDeadline = TaskNextPendingDeadline;

  struct node *tmp = curNode->next;
  curNode->next = link;
  link->next = tmp;
  link->prev = curNode;
}

void deleteNode(struct node* curNode) {
  curNode->next->prev = curNode->prev;

  curNode->prev->next = curNode->next;

  free(curNode);
}

/* ---------------------------------------------- */

static void ExecuteTask (Taskp t)
{
  t->Flags |= ACTIVE;

  t->Invoked++;
  t->Taskf(t->ExecutionTime); // execute task

  t->Flags &= (~ACTIVE);
}

void TaskArrange (Task Tasks[]) 
{
  if (head == NULL) {
    struct node *fakeHead = (struct node*) malloc(sizeof(struct node));
    fakeHead->TaskIdx = 0;
    fakeHead->TaskNextPendingDeadline = 0;

    struct node *fakeTail = (struct node*) malloc(sizeof(struct node));
    fakeTail->TaskIdx = 255;
    fakeTail->TaskNextPendingDeadline = 65535;

    fakeHead->prev = NULL;
    fakeHead->next = fakeTail;
    head = fakeHead;

    fakeTail->prev = fakeHead;
    fakeTail->next = NULL;
    tail = fakeTail;
  }

  uint8_t i;

  for (i = 0; i < NUMTASKS; i++) {
    Taskp CurTask = &(Tasks[i]);

    if (ScheduleTimes[i] != CurTask->Activated) {
      ScheduleTimes[i]++;

      struct node* traverseNode = head;

      while (traverseNode != tail) {
        if ((CurTask->NextPendingDeadline >= traverseNode->TaskNextPendingDeadline 
            && CurTask->NextPendingDeadline < traverseNode->next->TaskNextPendingDeadline)
            ||
            (CurTask->NextPendingDeadline >= traverseNode->TaskNextPendingDeadline 
            && CurTask->NextPendingDeadline == traverseNode->next->TaskNextPendingDeadline
            && i < traverseNode->next->TaskIdx)) 
        {
          insertNode(traverseNode, i, CurTask->NextPendingDeadline);
          break;
        }

        traverseNode = traverseNode->next;
      }

      CurTask->NextPendingDeadline += CurTask->Period;
    }
  }
}

void Scheduler_P_EDF (Task Tasks[])
{ 
  StartTracking(TT_SCHEDULER);

  SetLeds (BROWN, 1);

  TaskArrange (Tasks); 

  struct node* traverseNode = head->next;

  while (traverseNode != tail) {
    Taskp CurTask = &(Tasks[traverseNode->TaskIdx]);

    if (CurTask->Flags & ACTIVE) 
      break;

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

    struct node* next = traverseNode->next;
    deleteNode(traverseNode);
    
    traverseNode = next;
  }

  SetLeds (BROWN, 0);

  StopTracking(TT_SCHEDULER);
}
