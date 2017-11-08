/*
 *  Queue-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#include "pcs.h"
#include "queue.h"


stuct Queue* queueInit(){
    Queue * queue = (Queue *) malloc(sizeof(Queue));
    if(queue == NULL){
        return EXIT_FAILURE;
    }    
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

queueDestroy(struct Queue * queue){
    if(queue->tail != NULL){
        nodeFree(queue->head);
    }    
    free(queue);
}

nodeFree(Node * node){
    if(node->next != NULL){
        nodeFree(node->next);
    }
    free(node);
}    

