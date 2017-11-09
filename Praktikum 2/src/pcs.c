/*
 *  Main-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
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
#include "pcs.h"
#include "queue.h"
#include "queue_condi.h"
#include "queue_sema.h"



static bool debug = false;
static bool prod1Kill = false;
static bool prod2Kill = false;
static bool consumerKill = false;
static bool controlKill = false;

pthread_mutex_t mutex;
static pthread_mutex_t prod1Mutex;
static pthread_mutex_t prod2Mutex;
static pthread_mutex_t consumMutex;

static int (*queue_init)();
static void (*queueDestroy1)();
static int (*queueSetLoad)(char);
static char (*queueGetLoad)();
static int retProducer1, retProducer2, retConsumer, retControl;


void* producer1(void* unused){
    if(debug){
        printf("Producer1 ist gestartet\n");
    }
    int zaehler = 'a';
    while(!prod1Kill){
        if(debug){
            printf("Producer1 while anfang\n");
        }
        queueSetLoad(zaehler + '\0');
        zaehler++;
        if(zaehler == 1 + 'z'){
            zaehler = 'a';
        }
        if(debug){
            printf("Producer1 sleep\n");
        }
        sleep(SECONDS_TO_WAIT_3);
        pthread_mutex_lock(&prod1Mutex);
        pthread_mutex_unlock(&prod1Mutex);
    }
    retProducer1 = EXIT_SUCCESS;
    pthread_exit(&retProducer1);
}

void* producer2(void* unused){
    if(debug){
        printf("Producer2 ist gestartet\n");
    }
    int zaehler = 'A';
    while(!prod2Kill){
        if(debug){
            printf("Producer2 while anfang\n");
        }
        queueSetLoad(zaehler + '\0');
        zaehler++;
        if(zaehler == 1 + 'Z'){
            zaehler = 'A';
        }
        sleep(SECONDS_TO_WAIT_3);
        pthread_mutex_lock(&prod2Mutex);
        pthread_mutex_unlock(&prod2Mutex);
    }
    retProducer2 = EXIT_SUCCESS;
    pthread_exit(&retProducer2);
}

void* consumer(void* unused){
    retConsumer = EXIT_SUCCESS;
    while(!consumerKill){
        char tempChar = '\0';
        tempChar = queueGetLoad();
        if(tempChar == '\0'){
            printf("Consumerfehler: NULL-Pointer\n");
            retConsumer = EXIT_FAILURE;
            pthread_exit(&retConsumer);
        } else if (tempChar == '\n'){
        // TODO \n erstellen in den Queue's
            pthread_exit(&retConsumer);
        }
        printf("Ausgabe : %c\n", tempChar);
        sleep(SECONDS_TO_WAIT_2);
        pthread_mutex_lock(&consumMutex);
        pthread_mutex_unlock(&consumMutex);
    }


    pthread_exit(&retConsumer);
}

void* control(void* unused){
    bool prod1_blocked = false;
    bool prod2_blocked = false;
    bool consum_blocked = false;

    while(!controlKill){
        char c;
        printf("Eingabe: \n");
        c = getchar();
        switch (c){
            case '1':
                if(!prod1_blocked){
                    pthread_mutex_lock(&prod1Mutex);
                    prod1_blocked = true;
                    printf("Producer1 angehalten.\n");
                } else {
                    pthread_mutex_unlock(&prod1Mutex);
                    prod1_blocked = false;
                    printf("Producer1 gestartet.\n");
                }
                break;
            case '2':
                if(!prod2_blocked){
                    pthread_mutex_lock(&prod2Mutex);
                    prod2_blocked = true;
                    printf("Producer2 angehalten.\n");
                } else {
                    pthread_mutex_unlock(&prod2Mutex);
                    prod2_blocked = false;
                    printf("Producer2 gestartet.\n");
                }
                break;
            case 'c':
            case 'C':
                if(!consum_blocked){
                    pthread_mutex_lock(&consumMutex);
                    consum_blocked = true;
                    printf("Consumer angehalten.\n");
                } else {
                    pthread_mutex_unlock(&consumMutex);
                    consum_blocked = false;
                    printf("Consumer gestartet.\n");
                }
                break;
            case 'q':
            case 'Q':
                prod1Kill = true;
                pthread_mutex_unlock(&prod1Mutex);
                prod2Kill = true;
                pthread_mutex_unlock(&prod2Mutex);
                consumerKill = true;
                pthread_mutex_unlock(&consumMutex);
                controlKill = true;
                break;
            case 'h':
                printf("//TODO HILFETEXT 2!!!\n");
                break;
            case '\n':
                printf("test\n");
        }
    }
    retControl = EXIT_SUCCESS;
    pthread_exit(&retControl);
}

void helpText(){
    printf("psc ist ein Programm, welches ein Erzeuger-Verbraucher-System simuliert.\npsc [option]\n  otionen:\n    -s|-S -> Semaphore\n    -c|-C -> Conditional Variable\n");
    return;
}

void pointerInitSema(){
    queue_init = &queue_sema_init;
    queueDestroy1 = &destroy_sema_queue;
    queueSetLoad = &setLoad_sema;
    queueGetLoad = &getLoad_sema;

}

void pointerInitCondi(){
    queue_init = &queue_condi_init;
    queueDestroy1 = &destroy_condi_queue;
    queueSetLoad = &setLoad_condi;
    queueGetLoad = &getLoad_condi;
}

int main(int argc, char* argv[]){

    pthread_t threadArray[ANZAHL_THREADS];

    if(argc < 2){
        helpText();
        return EXIT_FAILURE;
    }

    switch (argv[1][1]){
        case 's':
        case 'S':
            pointerInitSema();
            break;
        case 'c':
        case 'C':
            pointerInitCondi();
            break;
        default:
            helpText();
            return EXIT_FAILURE;
    }

    pthread_mutex_init ( &mutex, NULL);
    pthread_mutex_init ( &prod1Mutex, NULL);
    pthread_mutex_init ( &prod2Mutex, NULL);
    pthread_mutex_init ( &consumMutex, NULL);

    for(int i = 0; i< ANZAHL_THREADS; i++){
        threadArray[i] = (pthread_t )i;
    }

    queue_init();

    int tempReturn = pthread_create( &(threadArray[0]), NULL, &control, NULL);
    if(tempReturn != 0){
        printf("Control konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    tempReturn = pthread_create( &(threadArray[1]), NULL, &producer1, NULL);
    if(tempReturn != 0){
        printf("Producer1 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    tempReturn = pthread_create( &(threadArray[2]), NULL, &producer2, NULL);
    if(tempReturn != 0){
        printf("Producer2 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    tempReturn = pthread_create( &(threadArray[3]), NULL, &consumer, NULL);
    if(tempReturn != 0){
        printf("Consumer konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }

    for(int i = 0; i < ANZAHL_THREADS; i++){
        pthread_join( threadArray[i], NULL);
    }

    // ALLes zerstören., freigeben.

    return EXIT_SUCCESS;
}


