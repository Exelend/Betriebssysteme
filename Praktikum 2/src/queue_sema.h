/*
 *  Queue-Semaphore-H-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#ifndef QUEUE_SEMA_H_
#define QUEUE_SEMA_H_

#define PRIVATE  0


int queue_sema_init(void);
int setLoad_sema(char load);
void destroy_sema_queue(void);
char getLoad_sema(void);
int end_queue_sema(void);


#endif //QUEUE_SEMA_H_
