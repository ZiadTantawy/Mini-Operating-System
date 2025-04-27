#include "../pcb.c"
#include "queue.c"
#include "scheduler.c"
// Assume these global
// extern PCBQueue readyQueue;
// extern PCB* runningProcess;

void scheduleFCFS() {
    int initialSize = getSize(&readyQueue);

    for (int i = 0; i < initialSize; i++) {
        PCB* runningPCB = dequeue(&readyQueue);
   
        updateState(runningPCB, RUNNING);

        // while (runningPCB->state != TERMINATED) {
        //     incrementPC(runningPCB);
        //     printf("Executing PID: %d | PC: %d\n", runningPCB->pid, runningProcess->programCounter);

        //     if (runningProcess->programCounter >= 5) {
        //         updateState(runningProcess, TERMINATED);
        //         printf("Process PID %d terminated.\n", runningProcess->pid);
        //     }
        // }

        // free(runningProcess);
        // runningProcess = NULL;
        //TODO: Call execute function for the process
    }
}
