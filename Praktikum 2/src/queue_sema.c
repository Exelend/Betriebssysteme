/*
 *  Queue-Semaphore-Datei, benoetigt queue.c.
 *  Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#include <semaphore.h>
#include "pcs.h"
#include "queue.h"

struct Queue* queue = NULL;
sem_t semaphorFull;
sem_t semaphorEmpty;

int queue_sema_init(){
    int returnValue = sem_init(semaphorEmpty, PRIVATE, BUFFERSIZE);
    if(returnValue < 0){
        return EXIT_FAILURE;
    }    
    returnValue = sem_init(semaphorFull, PRIVATE, 0);
    queue = queueInit();
    if(queue == NULL){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int setLoad_sema(char load){
    sem_wait(semaphorEmpty);
    NODE* node = (NODE*) malloc(sizeof(NODE));
    pthread_mutex_lock(&mutex);
    
    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    } else {
        node->next = tail;
        queue->tail = node;
    }
    
    pthread_mutex_unlock(&mutex);
    sem_post(semaphorFull);
    return EXIT_SUCCESS;
}  

char getLoad_sema(){
    sem_wait(semaphorFull);
    pthread_mutex_lock(&mutex);
    
    NODE* node = queue->head;
    queue->head = node->next;
    
    pthread_mutex_unlock(&mutex);
    sem_post(semaphorEmpty);
    char temp = node->load;
    free(node);
    node = NULL;
    return temp;
}    

destroy_sema_queue(){
    pthread_mutex_unlock(&mutex);
    sem_destroy(semaphorFull);
    sem_destroy(semaphorEmpty); 
    queueDestroy(queue);
    queue = NULL;
}



    

