#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "q.h"

typedef struct PriorityQueue {
    Node** heap;
    int capacity;
    int size;
    bool (*compare)(Node*, Node*);
} PriorityQueue;

bool compareByArrival(Node* a, Node* b) {
    // Define your comparison logic here
    // For example, compare based on arrival time
    return a->arrival > b->arrival;
}

PriorityQueue* createPriorityQueue(int capacity, bool (*compare)(Node*, Node*)) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->heap = (Node**)malloc(sizeof(Node*) * (capacity + 1));
    pq->capacity = capacity;
    pq->size = 0;
    pq->compare = compare;
    return pq;
}

void swap(Node** a, Node** b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(PriorityQueue* pq, int index) {
    while (index > 1 && pq->compare(pq->heap[index], pq->heap[index / 2])) {
        swap(&pq->heap[index / 2], &pq->heap[index]);
        index = index / 2;
    }
}

void enqueue_pq(PriorityQueue* pq, Node* node) {
    if (pq->size >= pq->capacity) {
        printf("Priority queue is full.\n");
        return;
    }

    pq->size++;
    pq->heap[pq->size] = node;
    heapifyUp(pq, pq->size);
}

void heapifyDown(PriorityQueue* pq, int index) {
    int smallest = index;
    int left = 2 * index;
    int right = 2 * index + 1;

    if (left <= pq->size && pq->compare(pq->heap[left], pq->heap[smallest])) {
        smallest = left;
    }

    if (right <= pq->size && pq->compare(pq->heap[right], pq->heap[smallest])) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&pq->heap[index], &pq->heap[smallest]);
        heapifyDown(pq, smallest);
    }
}

Node* dequeue_pq(PriorityQueue* pq) {
    if (pq->size <= 0) {
        printf("Priority queue is empty.\n");
        return NULL;
    }

    Node* root = pq->heap[1];
    pq->heap[1] = pq->heap[pq->size];
    pq->size--;
    heapifyDown(pq, 1);
    return root;
}

int isEmpty_pq(PriorityQueue* pq) {
    return pq->size == 0;
}

void destroyPriorityQueue(PriorityQueue* pq) {
    free(pq->heap);
    free(pq);
}

