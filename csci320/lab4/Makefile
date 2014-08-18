CC = gcc -I ./include
CFLAGS = -std=gnu99 -Wall -g #-DDEBUG

INC = ./include
SRC = ./src
OBJ = ./obj
DOC = ./doc
BIN = ./bin
DAT = ./data

vpath %.h ./include
vpath %.c ./src

EXECS = scoreboard

all: $(EXECS)

$(OBJ)/scoreboard.o: scoreboard.c
	$(CC) -c $(CFLAGS) $(SRC)/scoreboard.c -o $(OBJ)/scoreboard.o

scoreboard: $(OBJ)/scoreboard.o 
	$(CC) $(CFLAGS) -o $(BIN)/scoreboard $(OBJ)/scoreboard.o

.PHONY: clean
clean:
	/bin/rm -rf $(BIN)/* $(OBJ)/* core* *~
