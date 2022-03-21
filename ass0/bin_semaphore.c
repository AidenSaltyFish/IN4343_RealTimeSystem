/* includes */
#include <vxWorks.h>
#include <semLib.h>

int main() {
    SEM_ID semMutex;
    /* Create a binary semaphore that is initially full. Tasks *
    * blocked on semaphore wait in priority order. */
    semMutex = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

    semTake(semMutex, WAIT_FOREVER);
    /* critical region, only accessible by a single task at a time */
    semGive(semMutex);
}