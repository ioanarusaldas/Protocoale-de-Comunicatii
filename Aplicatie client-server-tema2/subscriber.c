//Savu Ioana Rusalda 325CB
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <math.h>
#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s <ID_Client> <IP_Server> <Port_Server>\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n = 0, ret = 0;
	bool stdin_command = false;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	fd_set read_fds;
	fd_set tmp_fds;
	int fdmax;

	// se goleste multimea de descriptori
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	//se verifica rulalrea clientului cu numarul de parametrii corect
	if (argc < 4) {
		usage(argv[0]);
	}
	//reare socket client
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "Client socket can't be created");

	//adaufare sockfd in multimea de descriptori
	FD_SET(sockfd, &read_fds);
	FD_SET(0, &read_fds);
	//actualizare fdmax
	fdmax = sockfd;
	//completare campuri structura sockaddr_in
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "Faild inet_aton function");

	//realizare conectare la server
	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "Connection faild");

	//trimitere id client la server pentru verificare unicitate
	n = send(sockfd, argv[1],strlen(argv[1]), 0);
	DIE(n < 0, "Send id falid");
	while (1) {
		tmp_fds = read_fds; 	
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select function for server faild");
		stdin_command = false;
		if (FD_ISSET(0, &tmp_fds)) { //citire comenzi de la tastatura
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN - 1, stdin);

			if (strncmp(buffer, "exit", 4) == 0) {
				printf("exit\n");
				break;
			}
			if (strncmp(buffer, "subscribe", 9) == 0) {
				int x = strlen(buffer) - 13;
				tcp_msg msg; //creare mesaj tcp pentru trimitere catre server
				memset(&msg,0,sizeof(msg));
				msg.type = 1;
				memcpy(msg.topic, &buffer[10], x * sizeof(char));
				msg.topic[x] = '\0';
				msg.SF = (int)(buffer[strlen(buffer) - 2] - 48);
				if ((msg.SF == 0) || (msg.SF == 1)) {
					n = send(sockfd, &msg, sizeof(msg), 0);
					DIE(n < 0, "send subscribe message faild");
					printf("subscribe %s\n",msg.topic);	
					stdin_command = true;
				}	
			}
			if (strncmp(buffer, "unsubscribe", 11) == 0) {
				int x = strlen(buffer) - 13;
				tcp_msg msg; //creare mesaj tcp pentru trimitere catre server
				memset(&msg,0,sizeof(msg));
				msg.type = 0;
				memcpy(msg.topic, &buffer[12], x * sizeof(char));
				msg.topic[x] ='\0';
				msg.SF = 0;
				n = send(sockfd, &msg, sizeof(msg), 0);
				DIE(n < 0, "send unsubscribe message faild");
				printf("unsubscribe %s\n",msg.topic);
				stdin_command = true;		
			}
			if (stdin_command == false) {
				printf ("Invalid command\n");
			}
		}
		if (FD_ISSET(sockfd, &tmp_fds)) {
  			udp_msg msg ;
  			memset(&msg,0,sizeof(msg));
			ret = recv(sockfd, &msg, sizeof(msg), 0);
			DIE(ret < 0, "receve message from server faild");
			if (strncmp(msg.topic, "exit", 4) == 0) {
				printf("%s\n", msg.topic);//eroare id 
				break;
			}
			if (strncmp(msg.topic, "You", 3) == 0) {
				printf("%s\n", msg.topic);//eroare unsubscribe
			} else {
				//cazul de interpretare a mesajelor udp
				//caz INT
				if((msg.data_type == 0) && (ntohs(msg.port )!= 0) ){
					uint32_t int_num = ntohl(*((uint32_t*)(msg.payload+ 1)));
					if(msg.payload[0] == 1) {

						   int_num *= -1;
					}

					printf("%s:%d - %s - INT - %d\n",inet_ntoa(msg.ip),
											ntohs(msg.port),msg.topic,int_num );

				}
				//caz SHORT_REAL
				if((msg.data_type == 1) && (ntohs(msg.port )!= 0) ){
				   	double real_num = ntohs(*(uint16_t*)(msg.payload));
            		real_num /= 100;
            		printf("%s:%d - %s - SHORT_REAL - %.2f\n",inet_ntoa(msg.ip),
            								ntohs(msg.port),msg.topic,real_num );
            	}
            	//caz FLOAT
				if((msg.data_type == 2) && (ntohs(msg.port )!= 0) ){
			        double real_num = ntohl(*(uint32_t*)(msg.payload + 1));
			        //unsigned p = ntohl(*(uint8_t*)(msg.payload + sizeof(uint32_t*)));
			        real_num /= pow(10,msg.payload[5]);
			            if (msg.payload[0] == 1) {
			                real_num *= -1;
			            }
			        printf("%s:%d - %s - FLOAT - %f\n",inet_ntoa(msg.ip),
			        						ntohs(msg.port),msg.topic,real_num );

            	}
            	//caz STRING
            	if((msg.data_type == 3) && (ntohs(msg.port )!= 0) ){
            		printf("%s:%d - %s - STRING - %s\n",inet_ntoa(msg.ip),
            							ntohs(msg.port),msg.topic,msg.payload);
            	}
            }

  		}
	}

	close(sockfd);
	return 0;
}