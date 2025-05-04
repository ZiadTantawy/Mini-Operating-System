#ifndef QUEUE_H
#define QUEUE_H

#include "pcb.h"  // For PCB definition


typedef struct Node {
    PCB pcb;
    struct Node *next;
} Node;
// PCB Queue structure
typedef struct {
    Node *front;
    Node *rear;
} PCBQueue;

// Queue operations
void initQueue(PCBQueue *q);
int isEmpty(PCBQueue *q);
void enqueue(PCBQueue *q, PCB pcb);
PCB dequeue(PCBQueue *q);          // Returns by value
PCB* peek(PCBQueue *q);           // Returns pointer to front PCB

#endif // QUEUE_H