/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

// Primeste date de pe connfd1 si trimite mesajul receptionat pe connfd2, la serverul de echo connfd1 == connfd2
int receive_and_send(int connfd1, int connfd2)
{
    char buf[BUFLEN];
    int bytes_send;
    int bytes_remaining;
    int bytes_received = 0;

    // TODO 6: Receptionati un mesaj de la connfd1
    bytes_received = read(connfd1, buf, BUFLEN);


    if (bytes_received != 0) {
        fprintf(stderr, "Received: %s", buf);
    }

    bytes_remaining = bytes_received;

    // TODO 7: Timiteti mesajul la connfd2
    bytes_send = write(connfd2, buf, bytes_received);


    return bytes_received;
}

void run_echo_server(int listenfd)
{
    struct sockaddr_in client_addr;

    int bytes_received;
    int connfd = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    // TODO 4: Ascultati pentru un singur client pe socketul dat
    listen(listenfd, 1);


    // TODO 5: Acceptati o conexiune
    connfd = accept(listenfd, (struct sockaddr*) &client_addr, &socket_len);
    if(connfd < 0){
        perror("accept client failed");
    }

    do {
        bytes_received = receive_and_send(connfd, connfd);
    } while (bytes_received > 0);

    // TODO 8: Inchideti conexiunea si socket-ul clientului
    close(connfd);
}

void run_chat_server(int listenfd)
{
    struct sockaddr_in client_addr1;
    struct sockaddr_in client_addr2;

    int bytes_received;
    int connfd1 = -1;
    int connfd2 = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    // TODO 4: Ascultati pentru doi clineti pe socketul dat
    listen(listenfd, 2);

    // TODO 5: Acceptati doua conexiuni
    connfd1 = accept(listenfd,(struct sockaddr*)&client_addr1, &socket_len);
    if (connfd1 < 0){
        perror("accept client failed");
    }
    connfd2 = accept(listenfd,(struct sockaddr*)&client_addr2, &socket_len) ;
    if (connfd2 < 0){
        perror("accept client failed");
    }

    do {
        bytes_received = receive_and_send(connfd1, connfd2);

        if (bytes_received == 0)
            break;

        bytes_received = receive_and_send(connfd2, connfd1);
    } while (bytes_received > 0);
    close(connfd1);
    close(connfd2);

    // TODO 8: Inchideti conexiunile si socketii creati
}

int main(int argc, char* argv[])
{
    int listenfd = -1;
    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);


    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // TODO 1: Creati un socket TCP pentru receptionarea conexiunilor
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
        perror("Socket creation failed");

    // TODO 2: Completati in serv_addr adresa serverului pentru bind, familia de adrese si portul rezervat pentru server
    struct sockaddr_in in;
    in.sin_family = AF_INET;
    in.sin_port = htons(12346);
    inet_aton("127.0.0.1", &in.sin_addr);


    // TODO 3: Asociati adresa serverului cu socketul creat folosind bind
   int b= bind(listenfd, (struct sockaddr*) &in, sizeof(struct sockaddr_in));
    if(b < 0){
        perror(" bind failed");

    }
   // run_echo_server(listenfd);
    run_chat_server(listenfd);


    // TODO 9: Inchideti socketul creat
    close(listenfd);

    return 0;
}
