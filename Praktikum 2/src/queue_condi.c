/*
 *  Queue-Conditional-Variable-Datei, benoetigt queue.c.
 *  Praktikumsaufgabe 2 in Betriebssysteme
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

static pthread_cond_t cv_producer;
static pthread_cond_t cv_consumer;
static struct Queue* queue = NULL;

int queue_condi_init(){
    int ret = pthread_cond_init(&cv_producer, NULL);
    if(ret != 0){
        return EXIT_FAILURE;
    }
    ret = pthread_cond_init(&cv_consumer, NULL);
    if(ret != 0){
        return EXIT_FAILURE;
    }
    queue = queueInit();
    if(queue == NULL){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void destroy_condi_queue(){
    pthread_mutex_unlock(&mutex);
    queueDestroy(queue);
    queue = NULL;
}

int setLoad_condi(char load){
    NODE* node = (NODE*) malloc(sizeof(NODE));
    node->load = load;
    node->next = NULL;
    pthread_mutex_lock(&mutex);
    while(queue->size >= BUFFERSIZE){
        pthread_cond_wait(&cv_producer, &mutex);
    }

    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    } else {
        (queue->tail)->next = node;
        queue->tail = node;
    }
    queue->size++;
    pthread_cond_signal(&cv_consumer);
    pthread_mutex_unlock(&mutex);
    return EXIT_SUCCESS;
}

char getLoad_condi(){
    pthread_mutex_lock(&mutex);
    while(queue->size <= 0){
        pthread_cond_wait(&cv_consumer, &mutex);
    }
    NODE* node = queue->head;
    queue->head = node->next;
    queue->size--;
    pthread_cond_signal(&cv_producer);
    pthread_mutex_unlock(&mutex);
    char temp = node->load;
    free(node);
    node = NULL;
    return temp;
}
