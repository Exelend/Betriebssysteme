/*
 *  Main-H-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#ifndef PCS_H_
#define PCS_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "queue_sema.h"
#include "queue_condi.h"
#include "queue.h"

#define BUFFERSIZE 10
#define ANZAHL_THREADS 4
#define SECONDS_TO_WAIT_3 3
#define SECONDS_TO_WAIT_2 2

pthread_mutex_t mutex;


#endif // PCH_H_
