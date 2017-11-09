/*
 *  Main-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */


#include "pcs.h"
#include <stdbool.h>

bool debug = false;
bool prod1Kill = false;
bool prod2Kill = false;
bool consumerKill = false;
bool controlKill = false;
pthread_t threadArray[ANZAHL_THREADS]; 

pthread_mutex_t mutex;
pthread_mutex_t prod1Mutex;
pthread_mutex_t prod2Mutex;
pthread_mutex_t consumMutex;

int (*queue_init) ();
void (*queueDestroy1) ();
int (*queueSetLoad)(char);
char (*queueGetLoad)();

void initProg(void){
    pthread_mutex_init ( &mutex, NULL);
    //mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init ( &prod1Mutex, NULL);
    //prod1Mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init ( &prod2Mutex, NULL);
    //prod2Mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init ( &consumMutex, NULL);
    //consumMutex = PTHREAD_MUTEX_INITIALIZER;
    return;
}

void producer1(){
    int zaehler = atoi('a');
    while(!prod1Kill){
        queueSetLoad(zaehler + '\0');
        zaehler++;
        if(zaehler == 1 + 'z'){
            zaehler = atoi('a');
        }    
        sleep(SECONDS_TO_WAIT_3);
        pthread_mutex_lock(&prod1Mutex);
        pthread_mutex_unlock(&prod1Mutex);
    }    
    return;
}    

void producer2(){
    int zaehler = atoi('A');
    while(!prod2Kill){
        queueSetLoad(zaehler + '\0');
        zaehler++;
        if(zaehler == 1 + 'Z'){
            zaehler = atoi('A');
        }    
        sleep(SECONDS_TO_WAIT_3);
        pthread_mutex_lock(&prod2Mutex);
        pthread_mutex_unlock(&prod2Mutex);
    }
    return;
} 

void consumer(){
    while(!consumerKill){
        char tempChar = '\0';
        tempChar = queueGetLoad();
        if(tempChar == '\0'){
            printf("Consumerfehler: NULL-Pointer");
            return;
        }    
        printf("Ausgabe : %c", tempChar);
        sleep(SECONDS_TO_WAIT_2);
        pthread_mutex_lock(&prod2Mutex);
        pthread_mutex_unlock(&prod2Mutex);
    }  
    
    return;
}    

void control(){
    bool prod1_blocked = false;
    bool prod2_blocked = false;
    bool consum_blocked = false;
    
    while(!controlKill){
        char c;
        printf("Eingabe: "); 
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
                } else {
                    pthread_mutex_unlock(&consumMutex);
                    consum_blocked = false;
                }    
                break;
            case 'q':
            case 'Q':    
                prod1Kill = true;
                prod2Kill = true;
                consumerKill = true;
                controlKill = true;
                break;    
            case 'h':
                printf("//TODO HILFETEXT 2!!!");
                break;
        }
    }
    return;
}    

void helpText(){
    printf("psc.c ist ein Programm das ein Erzeuger-Verbraucher-System simuliert.\npsc [option]\n  otionen:\n    -s|-S -> Semaphore\n    -c|-C -> Conditional Variable\n");
    return;
}    

int pointerInitSema(){
    queue_init = queue_sema_init;
    queueDestroy1 = destroy_sema_queue;
    queueSetLoad = setLoad_sema;
    queueGetLoad = getLoad_sema;
    
}   

int pointerInitCondi(){
    queue_init = queue_condi_init;
    queueDestroy1 = destroy_condi_queue;
    queueSetLoad = setLoad_condi;
    queueGetLoad = getLoad_condi;
}    

int main(int argc, char* argv[]){
    
    
    switch (argv[1][0]){
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
    
    initProg();
    queue_init();
    
    
    int tempReturn = pthread_create( &(threadArray[0]), NULL, control, NULL);
    if(tempReturn != 0){
        printf("Control konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    tempReturn = pthread_create( &(threadArray[1]), NULL, producer1, NULL);
    if(tempReturn != 0){
        printf("Producer1 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    tempReturn = pthread_create( &(threadArray[2]), NULL, producer2, NULL);
    if(tempReturn != 0){
        printf("Producer2 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    tempReturn = pthread_create( &(threadArray[3]), NULL, consumer, NULL);
    if(tempReturn != 0){
        printf("Producer1 konnte nicht gestartet werden!\n");
        return EXIT_FAILURE;
    }
    
    for(int i = 0; i < ANZAHL_THREADS, i++){
        pthread_join( threadArray[i], NULL);
    }    

    // ALLes zerstören., freigeben.
    
    return EXIT_SUCCESS;
}    


