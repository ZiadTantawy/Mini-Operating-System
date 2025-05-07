#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"

// Node for the queue
typedef struct Node {
    PCB pcb;
    struct Node *next;
} Node;

// PCB Queue structure
typedef struct {
    Node *front;
    Node *rear;
} PCBQueue;

// Initialize the queue
void initQueue(PCBQueue *q) {
    q->front = q->rear = NULL;
}

// Check if the queue is empty
int isEmpty(PCBQueue *q) {
    return (q->front == NULL);
}

// Enqueue a PCB into the queue
void enqueue(PCBQueue *q, PCB pcb) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->pcb = pcb;
    newNode->pcb.queueEntryTime = clockCycle;  // Record entry time
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    
    q->rear->next = newNode;
    q->rear = newNode;
}

// Dequeue a PCB from the queue -- **RETURN PCB VALUE NOT POINTER**
PCB dequeue(PCBQueue *q) {
    PCB dummyPCB;
    memset(&dummyPCB, 0, sizeof(PCB)); // Create empty PCB for errors

    if (isEmpty(q)) {
        printf("Queue is empty! Returning dummy PCB.\n");
        return dummyPCB;
    }

    Node *temp = q->front;
    PCB pcb = temp->pcb;  // COPY pcb VALUE

    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return pcb; // Return by value ✅
}

// Peek the front PCB without removing it
PCB* peek(PCBQueue *q) {
    if (isEmpty(q)) {
        return NULL;
    }
    return &(q->front->pcb); // peek returns pointer (safe)
}
