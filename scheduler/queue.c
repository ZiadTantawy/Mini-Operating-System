#include <stdio.h>
#include <stdlib.h>
#include "../pcb.c"

// ----------------- Structures -----------------


typedef struct Node {
    PCB pcb;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
    int size;
} PCBQueue;

// ----------------- Methods -----------------

// Initialize the queue
void initQueue(PCBQueue* q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

// Check if queue is empty
int isEmpty(PCBQueue* q) {
    return q->size == 0;
}

// Get size of the queue
int getSize(PCBQueue* q) {
    return q->size;
}

// Enqueue a PCB into the queue
void enqueue(PCBQueue* q, PCB pcb) {
    // Node* temp = (Node*)malloc(sizeof(Node));
    // if (temp == NULL) {
    //     printf("Memory allocation failed!\n");
    //     return;
    // }
    // temp->pcb = pcb;
    // temp->next = NULL;

    Node* current;
    current -> next = NULL;
    current->pcb = pcb;
    if (q->rear == NULL) {
        q->front = q->rear = current;
    } else {
        q->rear->next = current;
        q->rear = current;
    }
    q->size++;
}

// Dequeue a PCB from the queue
PCB* dequeue(PCBQueue* q) {
    PCB dummyPCB = {0};
    if (isEmpty(q)) {
        printf("Queue is empty! Returning dummy PCB.\n");
        return &dummyPCB;
    }

    Node* temp = q->front;
    PCB pcb = temp->pcb;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->size--;
    return &pcb;
}

// Peek at the front PCB without removing it
PCB* peek(PCBQueue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    return &(q->front->pcb);
}