/*
 *  Queue-Semaphore-Datei, benoetigt queue.c.
 *  Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#include <semaphore.h>
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

static struct Queue* queue = NULL;
static sem_t semaphorFull;
static sem_t semaphorEmpty;

int queue_sema_init(){
    int returnValue = sem_init(&semaphorEmpty, PRIVATE, BUFFERSIZE);
    if(returnValue < 0){
        return EXIT_FAILURE;
    }
    returnValue = sem_init(&semaphorFull, PRIVATE, 0);
    queue = queueInit();
    if(queue == NULL){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int setLoad_sema(char load){
    sem_wait(&semaphorEmpty);
    NODE* node = (NODE*) malloc(sizeof(NODE));
    node->load = load;
    node->next = NULL;
    pthread_mutex_lock(&mutex);

    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    } else {
        (queue->tail)->next = node;
        queue->tail = node;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&semaphorFull);
    return EXIT_SUCCESS;
}

char getLoad_sema(){
    sem_wait(&semaphorFull);
    pthread_mutex_lock(&mutex);

    if(consumerKill){
        return '\n';
    }
    
    NODE* node = queue->head;
    queue->head = node->next;

    pthread_mutex_unlock(&mutex);
    sem_post(&semaphorEmpty);
    char temp = node->load;
    free(node);
    node = NULL;
    return temp;
}

void destroy_sema_queue(){
    pthread_mutex_unlock(&mutex);
    sem_destroy(&semaphorFull);
    sem_destroy(&semaphorEmpty);
    queueDestroy(queue);
    queue = NULL;
}
