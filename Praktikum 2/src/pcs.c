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


// true für Debug(printf-Asgaben)
static bool debug = false;

//Variablen zum beenden der Threads
static bool prod1Kill = false;
static bool prod2Kill = false;
bool consumerKill = false;
static bool controlKill = false;

// Mutexe
pthread_mutex_t mutex;
static pthread_mutex_t prod1Mutex;
static pthread_mutex_t prod2Mutex;
static pthread_mutex_t consumMutex;

// Funktionspointer auf queue-funktionen.
static int (*queue_init)();
static void (*queueDestroy1)();
static int (*queueSetLoad)(char);
static char (*queueGetLoad)();

// 
static int retProducer1, retProducer2, retConsumer, retControl;

/*
 * Hilfetext Programmstart.
 */ 
void helpText(){
    printf("psc ist ein Programm, welches ein Erzeuger-Verbraucher-System simuliert.\npsc [option]\n  otionen:\n    -s|-S -> Semaphore\n    -c|-C -> Conditional Variable\n");
    return;
}

/*
 * Hilfetext bei laufendem Programm.
 */ 
void helpText2(){
    printf("Eingabe:\n1 -> Toggelt den Producer1\n2 -> Toggelt den Producer2\nc | C -> Toggelt den Consumer\nq | Q -> Beendet das Programm\nh -> zeigt alle Eingaben an");
}  

/*
 * Funktion erstellt kleinbuchstaben a-z, a-z, ... und übergiebt diese zur Speicherung an die Queue.
 */ 
void* producer1(void* unused){
    if(debug){
        printf("Producer1 ist gestartet\n");
    }
    // Variable zur erstellung der Buchstaben
    int zaehler = 'a';
    // Solange der Producer1 nicht beendet werden soll,....
    while(!prod1Kill){
        if(debug){
            printf("Producer1 while anfang\n");
        }
        // Buchstaben aus "zaehler" machen und an Queue übergeben;
        queueSetLoad(zaehler + '\0');
        // Buchstaben weiterzaehlen;
        zaehler++;
        // wenn der "zaehler" über z hinaus ist, fängt er wieder bei a an;
        if(zaehler == 1 + 'z'){
            zaehler = 'a';
        }
        if(debug){
            printf("Producer1 sleep\n");
        }
        // warten;
        sleep(SECONDS_TO_WAIT_3);
        // blockieren das Threads durch den control-Thread;
        pthread_mutex_lock(&prod1Mutex);
        pthread_mutex_unlock(&prod1Mutex);
    }
    // Beenden des Threads
    retProducer1 = EXIT_SUCCESS;
    pthread_exit(&retProducer1);
}

/*
 * Funktion erstellt kleinbuchstaben A-Z,A-Z, ... und übergiebt diese zur Speicherung an die Queue.
 */
void* producer2(void* unused){
    if(debug){
        printf("Producer2 ist gestartet\n");
    }
    // Variable zur erstellung der Buchstaben
    int zaehler = 'A';
    // Solange der Producer2 nicht beendet werden soll,....
    while(!prod2Kill){
        if(debug){
            printf("Producer2 while anfang\n");
        }
        // Buchstaben aus "zaehler" machen und an Queue übergeben;
        queueSetLoad(zaehler + '\0');
        // Buchstaben weiterzaehlen;
        zaehler++;
        // wenn der "zaehler" über Z hinaus ist, fängt er wieder bei A an;
        if(zaehler == 1 + 'Z'){
            zaehler = 'A';
        }
        // warten;
        sleep(SECONDS_TO_WAIT_3);
        // blockieren das Threads durch den control-Thread;
        pthread_mutex_lock(&prod2Mutex);
        pthread_mutex_unlock(&prod2Mutex);
    }
    // Beenden des Threads
    retProducer2 = EXIT_SUCCESS;
    pthread_exit(&retProducer2);
}

/*
 * Funktion holt aus der Queue einen Buchstaben und gibt ihn aus.
 */ 
void* consumer(void* unused){
    retConsumer = EXIT_SUCCESS;
    // Solange der Consumer-Thread nicht beendet werden soll, ....
    while(!consumerKill){
        // variable für ausgabe;
        char tempChar = '\0';
        // Buchstaben von der Queue hohlen;
        tempChar = queueGetLoad();
        // wenn es bei der Übernahme des char einen Fehler gab,...
        if(tempChar == '\0'){
            printf("Consumerfehler: NULL-Pointer\n");
            retConsumer = EXIT_FAILURE;
            pthread_exit(&retConsumer);
        } else if (tempChar == '\n'){
            // wenn der consumer beendet werden soll, .....
            retConsumer = EXIT_SUCCESS;
            pthread_exit(&retConsumer);
        }
        printf("Ausgabe : %c\n", tempChar);
        // warten;
        sleep(SECONDS_TO_WAIT_2);
        // blockieren das Threads durch den control-Thread;
        pthread_mutex_lock(&consumMutex);
        pthread_mutex_unlock(&consumMutex);
    }
    // beenden des Threads
    pthread_exit(&retConsumer);
}

/*
 * Funktion zum steuern des Programmes. Nimmt eingaben zur Laufzeit entgegen und "verwaltet" die Threads;
 */ 
void* control(void* unused){
    
    // Variablen zum speichern des Bloked-Zustandes der Threads;
    bool prod1_blocked = false;
    bool prod2_blocked = false;
    bool consum_blocked = false;

    printf("Eingabe: \n");
    // Solange der controll.Thread nicht beendet werden soll,.....
    while(!controlKill){
        // variable zum speichern der Konsoleneingabe;
        char c;
        // Warten auf Konsoleneingabe und speichern in der variable c;
        c = getchar();
        switch (c){
            case '1':
                // wenn der Producer1-Tread nicht geblockt ist, ....
                if(!prod1_blocked){
                    // Producer1 blocken;
                    pthread_mutex_lock(&prod1Mutex);
                    // Variable auf "geblockt setzten";
                    prod1_blocked = true;
                    printf("Producer1 angehalten.\n");
                } else { // wenn der Producer1 geblockt ist, .....
                    // Producer1 weiter laufen lassen;
                    pthread_mutex_unlock(&prod1Mutex);
                    // Variable auf "ungeblockt setzten";
                    prod1_blocked = false;
                    printf("Producer1 gestartet.\n");
                }
                break;
            case '2':
                // wenn der Producer2-Tread nicht geblockt ist, ....
                if(!prod2_blocked){
                    // Producer2 blocken;
                    pthread_mutex_lock(&prod2Mutex);
                    // Variable auf "geblockt setzten";
                    prod2_blocked = true;
                    printf("Producer2 angehalten.\n");
                } else { // wenn der Producer2 geblockt ist, .....
                    // Producer2 weiter laufen lassen;
                    pthread_mutex_unlock(&prod2Mutex);
                    // Variable auf "ungeblockt setzten";
                    prod2_blocked = false;
                    printf("Producer2 gestartet.\n");
                }
                break;
            case 'c':
            case 'C':
                // wenn der Consumer-Tread nicht geblockt ist, ....
                if(!consum_blocked){
                    // Consumer blockieren;
                    pthread_mutex_lock(&consumMutex);
                    // Variable auf "geblockt" setzten;
                    consum_blocked = true;
                    printf("Consumer angehalten.\n");
                } else { // wenn der Consumer-Thread geblockt ist, ....
                    // Consumer weterlaufen lassen;
                    pthread_mutex_unlock(&consumMutex);
                    // Variable auf "ungeblockt" setzten.
                    consum_blocked = false;
                    printf("Consumer gestartet.\n");
                }
                break;
            case 'q':
            case 'Q':
                // Prod1 soll bendet werden;
                prod1Kill = true;
                // Prod1 entsperren;
                pthread_mutex_unlock(&prod1Mutex);
                // Prod2 soll bendet werden;
                prod2Kill = true;
                // Prod2 entsperren;
                pthread_mutex_unlock(&prod2Mutex);
                // Consumer soll bendet werden;
                consumerKill = true;
                // Consumer entsperren;
                pthread_mutex_unlock(&consumMutex);
                // einen (leer-) eintrag in die Queue. Wenn die Queue leer währe, würde der Consumer bockiert bleiben.
                queueSetLoad('\n');
                // Selbstzerstörung einleiten
                controlKill = true;
                break;
            case 'h':
                helpText2();
                break;
        }
        if(c != '\n'){
            printf("Eingabe: (mit Return bestätigen)\n");
        }    
    }
    // Thread beenden
    retControl = EXIT_SUCCESS;
    pthread_exit(&retControl);
}

/*
 * Funktion initialisiert die Funktionspointer für die Semaphore-Implementierung
 */  
void pointerInitSema(){
    queue_init = &queue_sema_init;
    queueDestroy1 = &destroy_sema_queue;
    queueSetLoad = &setLoad_sema;
    queueGetLoad = &getLoad_sema;

}

/*
 * Funktion initialisiert die Funktionspointer für die Conditional-Variable-Implementierung
 */
void pointerInitCondi(){
    queue_init = &queue_condi_init;
    queueDestroy1 = &destroy_condi_queue;
    queueSetLoad = &setLoad_condi;
    queueGetLoad = &getLoad_condi;
}

int main(int argc, char* argv[]){
    // Array für die Thread-Id`s
    pthread_t threadArray[ANZAHL_THREADS];

    // wenn kein Argument mitgegeben wurde, ....
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

    // Mutexe initialisieren;
    pthread_mutex_init ( &mutex, NULL);
    pthread_mutex_init ( &prod1Mutex, NULL);
    pthread_mutex_init ( &prod2Mutex, NULL);
    pthread_mutex_init ( &consumMutex, NULL);

    // Array initialisieren; 
    for(int i = 0; i< ANZAHL_THREADS; i++){
        threadArray[i] = (pthread_t )i;
    }
    // Queue initialisieren;
    queue_init();
    // Threads starten und prüfen, ob sie gestartet sind.
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

    // warten auf Ende der Thraeds;
    for(int i = 0; i < ANZAHL_THREADS; i++){
        pthread_join( threadArray[i], NULL);
    }

    
    // Alles zerstören., freigeben.
    pthread_mutex_unlock(&mutex);
    queueDestroy1();
    pthread_mutex_destroy(&mutex);
    pthread_mutex_unlock(&prod1Mutex);
    pthread_mutex_destroy(&prod1Mutex);
    pthread_mutex_unlock(&prod2Mutex);
    pthread_mutex_destroy(&prod2Mutex);
    pthread_mutex_unlock(&consumMutex);
    pthread_mutex_destroy(&consumMutex);
    
    return EXIT_SUCCESS;
}


