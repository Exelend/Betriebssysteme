/*
 *  Queue-Conditional-Variable-Datei, benoetigt queue.c.
 *  Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */


#include "pcs.h"
#include "queue.h"

pthread_cond_t cv_producer;
pthread_cond_t cv_consumer;
struct Queue* queue = NULL;

int queue_condi_init(){
    cv_producer = PTHREAD_COND_INITIALIZER;
    cv_consumer = PTHREAD_COND_INITIALIZER;
    queue = queueInit();
    if(queue == NULL){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}    

destroy_condi_queue(){
    pthread_mutex_unlock(&mutex);
    queueDestroy(queue);
    queue = NULL;
}    

int setLoad_condi(char load){
    pthread_mutex_lock(&mutex);
    while(queue->size >= BUFFERSIZE){
        pthread_cond_wait(&cv_producer, &mutex)
    } 
    
    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    } else {
        node->next = tail;
        queue->tail = node;
    }
    queue->size++;
    cond_signal(&cv_consumer);
    pthread_mutex_unlock(&mutex);
    return EXIT_SUCCESS;
}    

char getLoad_condi(){
    pthread_mutex_lock(&mutex);
    while(queue->size <= 0){
        pthread_cond_wait(&cv_consumer, &mutex)
    } 
    NODE* node = queue->head;
    queue->head = node->next;
    queue->size--;
    cond_signal(&cv_producer);
    pthread_mutex_unlock(&mutex);
    char temp = node->load;
    free(node);
    node = NULL;
    return temp;
}    
