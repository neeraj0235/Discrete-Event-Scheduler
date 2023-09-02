#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Node.h"

// Define the Queue structure
typedef struct Queue {
    Node** data;
    int capacity;
    int front;
    int rear;
    int size;
} Queue;

// Function to create an empty Queue
Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->data = (Node**)malloc(sizeof(Node*) * capacity);
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    return queue;
}

// Function to enqueue a Node* pointer
void enqueue_q(Queue* queue, Node* newNode) {
    if (queue->size >= queue->capacity) {
        printf("Queue is full.\n");
        return;
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->data[queue->rear] = newNode;
    queue->size++;
}

// Function to dequeue a Node* pointer
Node* dequeue_q(Queue* queue) {
    if (queue->size <= 0) {
        return NULL; // Queue is empty
    }

    Node* frontNode = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    return frontNode;
}

// Function to check if the Queue is empty
bool isEmpty_q(Queue* queue) {
    return (queue->size == 0);
}

// Function to destroy the Queue and free memory
void destroyQueue(Queue* queue) {
    free(queue->data);
    free(queue);
}
