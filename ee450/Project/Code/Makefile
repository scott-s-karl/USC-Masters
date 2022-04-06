# Make File Setup
# Jacob Sorber tutorials referenced

# Setup complier
CC=gcc
CFLAGS=-g -Wall

all: myapp

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^

myapp: serverM.o serverA.o serverB.o serverC.o
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm *.o myapp
