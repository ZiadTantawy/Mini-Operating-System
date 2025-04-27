#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"

void scheduleRR()
{
    while (!isEmpty(&readyQueue))
    {
        PCB *runningPCB = dequeue(&readyQueue);
        updateState(runningPCB, RUNNING);
        // TODO: Call execute function for the process

    }
}
