#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Queue {

    int size;
    struct Node* head;

} Queue;


typedef struct Node {

    char data[256];
    struct Node* prev;
    struct Node* next;

} Node;




Queue* makeQueue() {

    Queue* q;
    q = malloc(sizeof(Queue));
    q->size = 0;
    q->head = NULL;
    return q;

}


int push(Queue* q, char* str) {

    if (q == NULL) {
        printf("queue error\n");
        return 0;
    }

    Node* node;
    node = malloc(sizeof(Node));
    node->next = NULL;
    node->prev = NULL;


    strcpy(node->data, str);

    if (q->size == 0) {

        q->head = node;

    } else {

        Node* iter = q->head;

        while (iter->next != NULL) {
            iter = iter->next;
        }

        iter->next = node;
        node->prev = iter;

    }

    q->size++;
    return 1;
}


char* pop(Queue* q) {

    if (q == NULL || q->size < 1) {

        return NULL;

    }

    char* str = malloc(256 * sizeof(char));

    if (str == NULL) {
        printf("error\n");
        exit(1);
    }

    if (q->size == 1) {

        strcpy(str, q->head->data);

        q->head = NULL;

    } else {

        strcpy(str, q->head->data);

        q->head = q->head->next;
        q->head->prev = NULL;

    }

    q->size--;
    return str;

}


int displayQueue(Queue* q) {

    if (q == NULL || q->size == 0) {
        printf("Queue is empty\n");
        return 0;
    }

    Node* iter = q->head;

    printf("%s,", iter->data);

    while (iter->next != NULL) {

        iter = iter->next;
        printf("%s,", iter->data);

    }

    printf("\n");
    return 1;

}

