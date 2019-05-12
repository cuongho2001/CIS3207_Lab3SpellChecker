#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue {
    int size;
    struct Node* head;
} Queue;


typedef struct Node {
    char data[256];
    struct Node* prev;
    struct Node* next;
} Node;


Queue* makeQueue();

int push( Queue* root, char* str );

char* pop( Queue* root );

int displayQueue( Queue* root );


#endif
