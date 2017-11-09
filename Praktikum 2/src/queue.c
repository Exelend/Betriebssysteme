/*
 *  Queue-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include "queue_sema.h"
#include "queue_condi.h"
#include "queue.h"
#include "pcs.h"


QUEUE* queueInit(){
    QUEUE* queue = (QUEUE *) malloc(sizeof(QUEUE));
    if(queue == NULL){
        return NULL;
    }
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void nodeFree(NODE* node){
    if(node->next != NULL){
        nodeFree(node->next);
    }
    free(node);
}

void queueDestroy(QUEUE* queue){
    if(queue->tail != NULL){
        nodeFree(queue->head);
    }
    free(queue);
}

