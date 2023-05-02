#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    MAX_DIRECTORY_LENGTH = 1024,
    MAX_QUEUE_SIZE = 100
};

typedef struct {
    char* data[MAX_QUEUE_SIZE];
    int front;
    int rear;
} Queue;

void InitQueue(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

int IsEmpty(Queue* q) {
    return q->front == q->rear;
}

int IsFull(Queue* q) {
    return q->rear == MAX_QUEUE_SIZE - 1;
}

bool Enqueue(Queue* q, char* data) {
    if (IsFull(q)) {
        return false;
    }
    q->data[++q->rear] = strdup(data);
    printf("\n%s Enqueued!!!!\n", data);
}

char* Dequeue(Queue* q) {
    if (IsEmpty(q)) {
        return NULL;
    }
    printf("\nDequeued!!!!\n");
    return q->data[++q->front];
}