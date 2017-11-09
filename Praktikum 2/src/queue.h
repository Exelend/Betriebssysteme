/*
 *  Queue-H-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct Node_t {
    char load;
    struct Node_t * next;
} NODE;

typedef struct Queue {
    NODE * head;
    NODE * tail;
    int size;
} QUEUE;


int queueInit(void);

int queueDestroy(struct Queue * queue);

#endif //QUEUE_H_
