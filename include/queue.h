#include <stdlib.h>
#include <string.h>

enum {
    MAX_DIRECTORY_LENGTH = 1024
};

typedef struct node 
{
    char* cwd;
    struct node *next;
} Node;
 
 
typedef struct Queue 
{
    Node* front;
    Node* rear; 
    int node_count; 
}Queue;
 
void initQueue(Queue *queue)
{
    queue->front = queue->rear = NULL; 
    queue->node_count = 0;
}
 
bool isEmpty(Queue *queue)
{
    return queue->node_count ? false : true; 
}
 
void enqueue(Queue *queue, char* cwd)
{
    Node* new_node = (Node *)malloc(sizeof(Node));
    new_node->cwd = cwd;
    new_node->next = NULL;
 
    if (isEmpty(queue))    // 큐가 비어있을 때
    {
        queue->front = new_node;       
    }
    else    // 비어있지 않을 때
    {
        queue->rear->next = new_node;    //맨 뒤의 다음을 new_node로 설정
    }
    queue->rear = new_node;    //맨 뒤를 new_node로 설정   
    queue->node_count++;    //큐안의 노드 개수를 1 증가
}
 
char* dequeue(Queue *queue)
{
    char *cwd = (char *)malloc(sizeof(char) * MAX_DIRECTORY_LENGTH);
    Node *ptr;
    if (isEmpty(queue))    //큐가 비었을 때
    {

        return NULL;
    }
    ptr = queue->front;    //맨 앞의 노드 ptr 설정 

    strcpy(cwd, ptr->cwd);
    queue->front = ptr->next;    //맨 앞은 ptr의 다음 노드로 설정
    free(ptr);    // ptr 해제 
    queue->node_count--;    //큐의 노드 개수를 1 감소
    
    return cwd;
}
