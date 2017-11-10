CC=gcc
CFLAGS=--std=c99 -pthread

all: pcs

pcs: pcs.o queue.o queue_condi.o queue_sema.o
	$(CC) $(CFLAGS) pcs.o queue.o queue_condi.o queue_sema.o -o pcs

pcs.o: pcs.c
	$(CC) $(CFLAGS) -c pcs.c

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c

queue_condi.o: queue_condi.c
	$(CC) $(CFLAGS) -c queue_condi.c

queue_sema.o: queue_sema.c
	$(CC) $(CFLAGS) -c queue_sema.c

clean:
	rn -rf *o pcs
