#define MAX_SIZE 100
#include "../pcb.c"

typedef struct Node {
    PCB pcb;
    struct Node* next;
} Node;

// PCB Queue
typedef struct {
    Node* front;
    Node* rear;
} PCBQueue;

// Initialize the queue
void initQueue(PCBQueue* q) {
    q->front = q->rear = NULL;
}

// Check if queue is empty
int isEmpty(PCBQueue* q) {
    return q->front == NULL;
}

// Enqueue a PCB
void enqueue(PCBQueue* q, PCB pcb) {
    // Node* temp = (Node*)malloc(sizeof(Node));
    // if (temp == NULL) {
    //     printf("Memory allocation failed!\n");
    //     return;
    // }
    // temp->pcb = pcb;
    // temp->next = NULL;
    Node* current = NULL;
    current->pcb = pcb;
    if (q->rear == NULL) {
        q->front = q->rear = current;
    } else {
        q->rear->next = current;
        q->rear = current;
    }
}

// Dequeue a PCB
PCB dequeue(PCBQueue* q) {
    PCB dummyPCB = {0}; // Return a dummy if queue is empty
    if (isEmpty(q)) {
        printf("Queue is empty! Returning dummy PCB.\n");
        return dummyPCB;
    }

    Node* temp = q->front;
    PCB pcb = temp->pcb;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return pcb;
}

// Peek at the front PCB
PCB* peek(PCBQueue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    return &(q->front->pcb);
}