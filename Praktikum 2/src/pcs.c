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




// true fuer Debug(printf-Asgaben)
static bool debug = false;

//Variablen zum beenden der Threads
bool prodKill = false;
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
static int (*end_queue)();

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
 * Funktion erstellt kleinbuchstaben a-z, a-z, ... und uebergiebt diese zur Speicherung an die Queue.
 */
void* producer1(void* unused){
    if(debug){
        printf("Producer1 ist gestartet\n");
    }
    // Variable zur Erstellung der Buchstaben
    int zaehler = 'a';
    // Solange der Producer1 nicht beendet werden soll,....
    while(!prodKill){
        if(debug){
            printf("Producer1 while anfang\n");
        }
        // Buchstaben aus "zaehler" machen und an Queue uebergeben;
        queueSetLoad(zaehler + '\0');
        // Buchstaben weiterzaehlen;
        zaehler++;
        // wenn der "zaehler" ueber z hinaus ist, faengt er wieder bei a an;
        if(zaehler == 1 + 'z'){
            zaehler = 'a';
        }
        if(debug){
            printf("Producer1 sleep\n");
        }
        // warten;
        sleep(SECONDS_TO_WAIT_3);
        // blockieren des Threads durch den control-Thread;
        pthread_mutex_lock(&prod1Mutex);
        pthread_mutex_unlock(&prod1Mutex);
    }
    // Beenden des Threads
    retProducer1 = EXIT_SUCCESS;
    pthread_exit(&retProducer1);
}

/*
 * Funktion erstellt Kleinbuchstaben A-Z,A-Z, ... und uebergiebt diese zur Speicherung an die Queue.
 */
void* producer2(void* unused){
    if(debug){
        printf("Producer2 ist gestartet\n");
    }
    // Variable zur Erstellung der Buchstaben
    int zaehler = 'A';
    // Solange der Producer2 nicht beendet werden soll,....
    while(!prodKill){
        if(debug){
            printf("Producer2 while anfang\n");
        }
        // Buchstaben aus "zaehler" machen und an Queue uebergeben;
        queueSetLoad(zaehler + '\0');
        // Buchstaben weiterzaehlen;
        zaehler++;
        // wenn der "zaehler" ueber Z hinaus ist, faengt er wieder bei A an;
        if(zaehler == 1 + 'Z'){
            zaehler = 'A';
        }
        // warten;
        sleep(SECONDS_TO_WAIT_3);
        // blockieren des Threads durch den control-Thread;
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
        // Variable fuer ausgabe;
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
        // blockieren des Threads durch den control-Thread;
        pthread_mutex_lock(&consumMutex);
        pthread_mutex_unlock(&consumMutex);
    }
    // beenden des Threads
    pthread_exit(&retConsumer);
}

/*
 * Funktion zum steuern des Programmes. Nimmt Eingaben zur Laufzeit entgegen und "verwaltet" die Threads;
 */
void* control(void* unused){

    // Variablen zum speichern des Bloked-Zustandes der Threads;
    bool prod1_blocked = false;
    bool prod2_blocked = false;
    bool consum_blocked = false;

    printf("Eingabe: \n");
    // Solange der controll-Thread nicht beendet werden soll,.....
    while(!controlKill){
        // variable zum speichern der Konsoleneingabe;
        char c;
        // Warten auf Konsoleneingabe und speichern in der Variable c;
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
            //Selbstzerstoerungssequence einleiten ;)
            case 'q':
            case 'Q':
                // Prod1 soll beendet werden;
                prodKill = true;
                // Prod1 entsperren;
                pthread_mutex_unlock(&prod1Mutex);
                end_queue();
                // Prod2 entsperren;
                pthread_mutex_unlock(&prod2Mutex);
                end_queue();
                
                // Consumer soll bendet werden;
                consumerKill = true;
                // Consumer entsperren;
                pthread_mutex_unlock(&consumMutex);
                // einen (leer-) eintrag in die Queue. Wenn die Queue leer waehre, wuerde der Consumer bockiert bleiben.
                queueSetLoad('\n');
                // Selbstzerstoerung einleiten
                controlKill = true;
                break;
            case 'h':
                helpText2();
                break;
        }
        if(c != '\n'){
            printf("Eingabe: (mit Return bestaetigen)\n");
        }
    }
    // Thread beenden
    retControl = EXIT_SUCCESS;
    pthread_exit(&retControl);
}

/*
 * Funktion initialisiert die Funktionspointer fuer die Semaphore-Implementierung
 */
void pointerInitSema(){
    queue_init = &queue_sema_init;
    queueDestroy1 = &destroy_sema_queue;
    queueSetLoad = &setLoad_sema;
    queueGetLoad = &getLoad_sema;
    end_queue = &end_queue_sema;

}

/*
 * Funktion initialisiert die Funktionspointer fuer die Conditional-Variable-Implementierung
 */
void pointerInitCondi(){
    queue_init = &queue_condi_init;
    queueDestroy1 = &destroy_condi_queue;
    queueSetLoad = &setLoad_condi;
    queueGetLoad = &getLoad_condi;
    end_queue = &end_queue_condi;
}

int main(int argc, char* argv[]){
    // Array fuer die Thread-Id`s
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
    // Threads starten und pruefen, ob sie gestartet sind.
    int tempReturn = pthread_create( &(threadArray[0]), NULL, &control, NULL);
    FEHLERBEHANDLUNG(tempReturn, "Control konnte nicht gestartet werden!\n");
    
    tempReturn = pthread_create( &(threadArray[1]), NULL, &producer1, NULL);
    FEHLERBEHANDLUNG(tempReturn, "Producer1 konnte nicht gestartet werden!\n");
    
    tempReturn = pthread_create( &(threadArray[2]), NULL, &producer2, NULL);
    FEHLERBEHANDLUNG(tempReturn, "Producer2 konnte nicht gestartet werden!\n");
    
    tempReturn = pthread_create( &(threadArray[3]), NULL, &consumer, NULL);
    FEHLERBEHANDLUNG(tempReturn, "Consumer konnte nicht gestartet werden!\n");
    

    // warten auf Ende der Thraeds;
    for(int i = 0; i < ANZAHL_THREADS; i++){
        pthread_join( threadArray[i], NULL);
    }


    // Alles zerstoeren, freigeben;
    FEHLERBEHANDLUNG(pthread_mutex_unlock(&mutex) < 0, "Mutex der Queue konnte nicht ungelockt werden!\n");
    queueDestroy1();
    FEHLERBEHANDLUNG(pthread_mutex_unlock(&mutex) < 0, "x Mutex der Queue konnte nicht ungelockt werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_destroy(&mutex) < 0, "Main: Mutex der Queue konnte nicht freigegeben werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_unlock(&prod1Mutex) < 0, "Mutex Prod1 konnte nicht ungelockt werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_destroy(&prod1Mutex) < 0, "Mutex Prod1 konnte nicht freigegeben werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_unlock(&prod2Mutex) < 0, "Mutex Prod2 konnte nicht ungelockt werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_destroy(&prod2Mutex) < 0, "Mutex Prod2 konnte nicht freigegeben werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_unlock(&consumMutex) < 0, "Mutex Consumer konnte nicht ungelockt werden!\n");
    FEHLERBEHANDLUNG(pthread_mutex_destroy(&consumMutex) < 0, "Mutex Consumer konnte nicht freigegeben werden!\n");
 
    return EXIT_SUCCESS;
}


