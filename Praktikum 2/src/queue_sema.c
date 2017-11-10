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

/*
 * Initialisier eine Queue mit dazu gehoerigen Semaphoren.
 */
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

/*
 * Erstellt fur den uebergebenen Char ein Node und fuegt diesen in die Queue ein.
 */
int setLoad_sema(char load){
    //Thread blockiert, wenn die Queue leer ist
    sem_wait(&semaphorEmpty);
    //Node wird erstellt und gefuellt
    NODE* node = (NODE*) malloc(sizeof(NODE));
    node->load = load;
    node->next = NULL;
    //Kritischer Abschnitt Anfang
    pthread_mutex_lock(&mutex);

    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    } else {
        (queue->tail)->next = node;
        queue->tail = node;
    }

    //Kritischer Abschnitt Ende
    pthread_mutex_unlock(&mutex);
    //Teilt dem Consumer mit, dass ein neues Element vorhanden ist
    sem_post(&semaphorFull);
    return EXIT_SUCCESS;
}

/*
 * Erstellt fur den uebergebenen Char ein Node und fuegt diesen in die Queue ein.
 */
char getLoad_sema(){
    //Ueberpruefung ob ueberhaupt ein Element in der Queue, wenn nicht blockiert der Thread
    sem_wait(&semaphorFull);
    //Kritischer Abschnitt Anfang
    pthread_mutex_lock(&mutex);
    //Ueberpruefung ob der Thread sterben soll
    if(consumerKill){
        return '\n';
    }

    NODE* node = queue->head;
    queue->head = node->next;

    //Kritischer Abschnitt Ende
    pthread_mutex_unlock(&mutex);
    //Den Produzern mitteilen, dass wieder Platz in der Queue ist
    sem_post(&semaphorEmpty);
    //Den Char auslesen und den Node freibebn
    char temp = node->load;
    free(node);
    node = NULL;
    return temp;
}

/*
 * Zerstoert die Semaphoren und die Queue.
 */
void destroy_sema_queue(){
    pthread_mutex_unlock(&mutex);
    sem_destroy(&semaphorFull);
    sem_destroy(&semaphorEmpty);
    queueDestroy(queue);
    queue = NULL;
}
