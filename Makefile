CC = g++
CFLAGS = -g -Wall -I.
OBJ = jstrings.o main.o

%.o: %.cpp %.h
	$(CC) -c -o $@ $< $(CFLAGS)

default: jstrings

jstrings: main.o jstrings.o
	$(CC) $(CFLAGS) -o jstrings main.o jstrings.o



