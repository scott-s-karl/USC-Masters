# Steven Karl
# Makefile
# -----------------

############################
# wildcard explained
# (wildcard pattern...) - Gets a list of all files that match said pattern
############################

############################
# patsubst explained
# (patsubst pattern, replacement, text) - finds the whitespace separated
# words in "text" that match "pattern" and replaces them with replacement
############################

############################
# @/< explained
# @ - is the target name so in the case of (BINSI) the value is BINSI
# which is actually BINSI=(BIN)/scaleImage
# < - The filename of the first prerequiste
############################

# Setup Local Variables
CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
BIN=bin
HDR=hdr
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BINSI=$(BIN)/scaleImage

all: $(BINSI)

$(BINSI): $(OBJ)/scaleImage.o
	$(CC) $(CFLAGS) $< -o $@
$(OBJ)/%.o: $(SRC)/%.c $(HDR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	$(RM) -r $(BIN)/* $(OBJ)/*
