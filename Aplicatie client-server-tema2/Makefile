# Protocoale de comunicatii:
# Laborator 8: Multiplexare
# Makefile
CC = gcc -lm
CFLAGS = -Wall -g -lm

all: server subscriber

# Compileaza server.c
server: server.c -lm

# Compileaza subscriber.c
subscriber: subscriber.c -lm

.PHONY: clean run_server run_subscriber

clean:
	rm -f server subscriber
