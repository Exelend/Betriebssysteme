/*
 *  Main-H-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#ifndef PCS_H_
#define PCS_H_



#define BUFFERSIZE 10
#define ANZAHL_THREADS 4
#define SECONDS_TO_WAIT_3 3
#define SECONDS_TO_WAIT_2 2

extern pthread_mutex_t mutex;
extern bool consumerKill;


#endif // PCH_H_
