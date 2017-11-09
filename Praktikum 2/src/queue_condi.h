/*
 *  Queue-Conditional-Variable-H-Datei zur Praktikumsaufgabe 2 in Betriebssysteme
 *  06.11.2017
 *  Hauke Goldhammer 2286057
 *  Martin Witte 2285586
 */

#ifndef QUEUE_CONDI_H_
#define QUEUE_CONDI_H_



int queue_condi_init(void);
void destroy_condi_queue(void);
int setLoad_condi(char load);
char getLoad_condi(void);


#endif // QUEUE_CONDI_H_
