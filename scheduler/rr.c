#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"


void scheduleRR() {
    int initialSize = getSize(&readyQueue);
    
    for (int i = 0; i < initialSize; i++) {
        PCB* runningPCB = dequeue(&readyQueue);
        updateState(runningPCB, RUNNING);
        
      
    }
}
