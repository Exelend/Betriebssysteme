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


/*
 * Initialisiert eine Queue mit dazu gehoerigen conditional Variables.
 */
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

int end_queue_condi(void){
    queue->size = 0;
    pthread_cond_signal(&cv_producer);
}

/*
 * Zerstoert die conditional Variables und die Queue.
 */
void destroy_condi_queue(){
	FEHLERBEHANDLUNG(pthread_mutex_unlock(&mutex) < 0, "Queue Mutex konnte nicht ungelockt werden!\n");
	FEHLERBEHANDLUNG(pthread_cond_destroy(&cv_consumer)<0,"");
	FEHLERBEHANDLUNG(pthread_cond_destroy(&cv_producer)<0,"");
    queueDestroy(queue);
    queue = NULL;
}

/*
 * Erstellt fuer den uebergebenen Char einen Node und fuegt diesen in die Queue ein.
 */
int setLoad_condi(char load){
    //Node wird erstellt und befuellt;
    NODE* node = (NODE*) malloc(sizeof(NODE));
    node->load = load;
    node->next = NULL;
    //Kritischer Abschnitt Anfang
    pthread_mutex_lock(&mutex);
    //Ueberpruefung ob noch Platz in der Queue ist, wenn nicht blockiert der Thread.
    while(queue->size >= BUFFERSIZE){
        pthread_cond_wait(&cv_producer, &mutex);
        if(prodKill){
        	pthread_mutex_unlock(&mutex);
            return EXIT_SUCCESS;
        }    
    }
    // wenn noch kein NODE  in der Queue ist, ....
    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    } else { // wenn Nodes > 0 in der Queue, .... 
        (queue->tail)->next = node;
        queue->tail = node;
    }
    queue->size++;
    //Kritischer Abschnitt Ende
    pthread_mutex_unlock(&mutex);
    //Teilt dem Consumer mit, dass ein neues Element vorhanden ist
    pthread_cond_signal(&cv_consumer);
    

    return EXIT_SUCCESS;
}

/*
 * Holt einen Node aus dem Queue und gibt den Inhalt (char) zurück. Der Node wird zerstört.
 */ 
char getLoad_condi(){
    char temp = '\0';
    //Kritischer Abschnitt Anfang
    pthread_mutex_lock(&mutex);
    //Ueberpruefung ob Elemente in der Queue sind
    while(queue->size <= 0){
        pthread_cond_wait(&cv_consumer, &mutex);
        //Ueberpruefung ob der Thread sterben soll
        if(consumerKill){
            return '\n';
        }
    }
    //Erstes Element rausnehmen und das naechste Element dem Head zuweisen
    NODE* node = queue->head;
    queue->head = node->next;
    queue->size--;
    
    pthread_mutex_unlock(&mutex);
    //Den Produzern mitteilen, dass wieder Platz in der Queue ist
    pthread_cond_signal(&cv_producer);
    //Den Char auslesen und den Node freibebn
    temp = node->load;
    free(node);
    node = NULL;
    return temp;
}
