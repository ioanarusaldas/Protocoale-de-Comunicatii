//Savu Ioana Rusalda 325CB
#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>

/*
 * Macro de verificare a erorilor
 * Exemplu:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

// modificat - initial era 256
#define BUFLEN		1560	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	5	// numarul maxim de clienti in asteptare
#define LEN 50

typedef struct udp{
	char topic[50];
    unsigned int data_type;
    char payload[1500];
}udp;
typedef struct tcp_message {
    unsigned char type;
    char topic[LEN];
    unsigned char SF;
} tcp_msg;
typedef struct udp_message {
    char topic[50];
    unsigned int data_type;
    char payload[1500];
   	struct in_addr ip;
	unsigned short int port;
} udp_msg;

struct Map {
    char topic[LEN];
    int nr_subscribers;
    struct client *subscribers;
};

struct client {
	int client_socket;
	char id[10];
	int SF;
};
struct buffer_for_message {
	char id[10];
	int nr_msg;
	struct udp_message *msg;
};

#endif
