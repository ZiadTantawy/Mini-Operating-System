#include "../pcb.c"
#include "queue.c"
#include "scheduler.c"
// Assume these global
// extern PCBQueue readyQueue;
// extern PCB* runningProcess;

void scheduleFCFS()
{

    while (!isEmpty(&readyQueue))
    {
        PCB *runningPCB = dequeue(&readyQueue);
        updateState(runningPCB, RUNNING);
        // TODO: Call execute function for the process
    }
}
