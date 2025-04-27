#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h" // Include the header instead of pcb.c

typedef struct Node
{
    PCB pcb;
    struct Node *next;
} Node;

typedef struct
{
    Node *front;
    Node *rear;
} PCBQueue;

void initQueue(PCBQueue *q)
{
    q->front = q->rear = NULL;
}

int isEmpty(PCBQueue *q)
{
    return (q->front == NULL);
}

void enqueue(PCBQueue *q, PCB pcb)
{
    Node *current = (Node *)malloc(sizeof(Node));
    if (current == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    current->pcb = pcb;
    current->next = NULL;

    if (q->rear == NULL)
    {
        q->front = q->rear = current;
    }
    else
    {
        q->rear->next = current;
        q->rear = current;
    }
}

PCB dequeue(PCBQueue *q)
{
    PCB dummyPCB = {0}; // Initialize all fields to 0

    if (isEmpty(q))
    {
        printf("Queue is empty! Returning dummy PCB.\n");
        return dummyPCB;
    }

    Node *temp = q->front;
    PCB pcb = temp->pcb;

    q->front = q->front->next;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);
    return pcb;
}

PCB *peek(PCBQueue *q)
{
    if (isEmpty(q))
    {
        return NULL;
    }
    return &(q->front->pcb);
}