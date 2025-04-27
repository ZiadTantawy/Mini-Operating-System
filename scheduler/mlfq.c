#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"


PCBQueue prioirty1;
PCBQueue prioirty2;
PCBQueue prioirty3;
PCBQueue prioirty4;

void scheduleMLFQ(PCBQueue* levels[], PCB** runningProcess, int* timeSliceRemaining) {
    int initialSize = getSize(&readyQueue);
    
    for (int i = 0; i < initialSize; i++) {
        PCB* runningPCB = dequeue(&readyQueue);
        updateState(runningPCB, RUNNING);
        
      
    }
}
