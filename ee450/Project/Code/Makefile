# Make File Setup
# Jacob Sorber tutorials referenced

# Setup complier
CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
BIN=bin
HDR=header
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BINSM=$(BIN)/serverM
BINSA=$(BIN)/serverA
BINSB=$(BIN)/serverB
BINSC=$(BIN)/serverC
BINCA=$(BIN)/clientA
BINCB=$(BIN)/clientB

all: $(BINSM) $(BINSA) $(BINSB) $(BINSC) $(BINCA) $(BINCB)

$(BINSM): $(OBJ)/serverM.o
	$(CC) $(CFLAGS) $< -o $@ 
$(BINSA): $(OBJ)/serverA.o
	$(CC) $(CFLAGS) $< -o $@ 
$(BINSB): $(OBJ)/serverB.o
	$(CC) $(CFLAGS) $< -o $@ 
$(BINSC): $(OBJ)/serverC.o
	$(CC) $(CFLAGS) $< -o $@ 
$(BINCA): $(OBJ)/clientA.o
	$(CC) $(CFLAGS) $< -o $@ 
$(BINCB): $(OBJ)/clientB.o
	$(CC) $(CFLAGS) $< -o $@
$(OBJ)/%.o: $(SRC)/%.c $(HDR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	$(RM) -r $(BIN)/* $(OBJ)/* 
