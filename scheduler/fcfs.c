#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"
 


void scheduleFCFS(PCBQueue* readyQueue, PCB** runningProcess) {
    if (*runningProcess == NULL) {
        *runningProcess = dequeue(readyQueue);
        if (*runningProcess != NULL) {
            updateState(*runningProcess, RUNNING);
        }
    }

    if (*runningProcess != NULL) {
        incrementPC(*runningProcess);

        // Example logic: process terminates after 5 instructions
        if ((*runningProcess)->programCounter >= 5) {
            updateState(*runningProcess, TERMINATED);
            *runningProcess = NULL;
        }
    }
}
