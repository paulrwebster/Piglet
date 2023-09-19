# Makefile for Piglet chess
# *****************************************************
# Run command mingw32-make piglet
# *****************************************************
#variables to control the makefile operation
CC = g++
CFLAGS = -std=c++20 -pthread
# ****************************************************
# Targets needed to bring the executable up to date
piglet : piglet.o moveGen.o evaluation.o search.o hashing.o gameboard.o fen.o
	$(CC) $(CFLAGS) -o piglet piglet.o moveGen.o evaluation.o search.o hashing.o gameboard.o fen.o
piglet.o : piglet.cpp moveGen.h evaluation.h search.h hashing.h gameboard.h fen.h
	$(CC) $(CFLAGS) -c piglet.cpp
moveGen.o : moveGen.cpp moveGen.h evaluation.h defs.h
	$(CC) $(CFLAGS) -c moveGen.cpp
evaluation.o : evaluation.cpp evaluation.h
	$(CC) $(CFLAGS) -c evaluation.cpp
fen.o : fen.cpp fen.h
	$(CC) $(CFLAGS) -c fen.cpp
gameboard.o : gameboard.cpp gameboard.h evaluation.h moveGen.h hashing.h
	$(CC) $(CFLAGS) -c gameboard.cpp
hashing.o : hashing.cpp hashing.h gameboard.h moveGen.h
	$(CC) $(CFLAGS) -c hashing.cpp
search.o : search.cpp search.h defs.h moveGen.h evaluation.h hashing.h
	$(CC) $(CFLAGS) -c search.cpp
clean :
	rm piglet piglet.o moveGen.o evaluation.o search.o hashing.o gameboard.o fen.o
