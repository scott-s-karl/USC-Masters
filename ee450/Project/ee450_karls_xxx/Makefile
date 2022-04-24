# Make File Setup
# Jacob Sorber tutorials referenced

# Setup complier
CC=gcc
CFLAGS=-g -Wall


all: serverM serverA serverB serverC clientA clientB

serverM: serverM.o
	$(CC) $(CFLAGS) $< -o $@
serverA: serverA.o
	$(CC) $(CFLAGS) $< -o $@
serverB: serverB.o
	$(CC) $(CFLAGS) $< -o $@
serverC: serverC.o
	$(CC) $(CFLAGS) $< -o $@
clientA: clientA.o
	$(CC) $(CFLAGS) $< -o $@
clientB: clientB.o
	$(CC) $(CFLAGS) $< -o $@


serverM.o: serverM.c serverM.h
	$(CC) $(CFLAGS) -c $< -o $@

serverA.o: serverA.c serverA.h
	$(CC) $(CFLAGS) -c $< -o $@

serverB.o: serverB.c serverB.h
	$(CC) $(CFLAGS) -c $< -o $@

serverC.o: serverC.c serverC.h
	$(CC) $(CFLAGS) -c $< -o $@

clientA.o: clientA.c clientA.h
	$(CC) $(CFLAGS) -c $< -o $@

clientB.o: clientB.c clientB.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm *.o* *~ *.exe* 
