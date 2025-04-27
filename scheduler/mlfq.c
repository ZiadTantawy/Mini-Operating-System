#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"

PCBQueue prioirty1;
PCBQueue prioirty2;
PCBQueue prioirty3;
PCBQueue prioirty4;

void scheduleMLFQ()
{
    while (!isEmpty(&readyQueue))
    {
        PCB *runningPCB = dequeue(&readyQueue);
        updateState(runningPCB, RUNNING);
        // TODO: Call execute function for the process
    }
}
