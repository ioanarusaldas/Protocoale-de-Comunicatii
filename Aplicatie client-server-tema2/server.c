//Savu Ioana Rusalda 325CB
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include "helpers.h"
#include "function.c"


void usage(char *file)
{
	fprintf(stderr, "Arguments needed: %s <PORT_DORIT>\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	/*
		sockfd_tcp_tcp -socket pentru clientii tcp
		sockfd_tcp_udp -socket pentru clientii udp
		new_client -socket client nou conectat
		protno -numar port
	*/
	int sockfd_tcp, new_client, portno,sockfd_udp;
	char buffer[BUFLEN];
	/*
		map- mapare intre topic si clientii abonati la topicul respectiv
		connected -vector clienti conectati
		save -vector mesaje salvate pentru clientii deconectati(caz SF = 1)
		new_connected -vector auxiliar folosit in realocare
		new_client_connected -structura client nou conectat
		
	*/
	struct Map *map = NULL;
	struct client *connected = NULL;
	struct buffer_for_message *save = NULL;
	struct client *new_connected = NULL;
	//struct client *new_disconnected = NULL;
	

	map = malloc(sizeof(struct Map));
	connected = malloc(sizeof(struct client));
	save = malloc(sizeof(struct buffer_for_message));
	//disconnected = malloc(sizeof(struct client));
	// structuri pentru server , clientTCP si clientUDP
	struct sockaddr_in serv_addr, cli_addr,cli_udp;
	/*
		msg_saved -numar mesaje salvate
		nr_topics -numar topicuri din map
		nr_clients -numar clienti conectati
	*/
	int n, i, ret = -1,nr_clients = 0,nr_topics = 0,msg_saved = 0;
	int index = -1;
	bool new = false;
	socklen_t clilen;
	socklen_t udplen;
	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	//verificare rulare server cu argumente corecte
	if (argc < 2) {
		usage(argv[0]);
	}
	//generare port number
	portno = atoi(argv[1]);
	DIE(portno == 0, "Atoi for portno faild");

	//se goleste multimea de descriptori 
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	//creare socket pasiv tcp
	sockfd_tcp = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd_tcp < 0, "Socket TCP can't be created");
	//dezactivare algoritm Neagle
	int flag = -1;
	int result = setsockopt(sockfd_tcp, IPPROTO_TCP, TCP_NODELAY, 
												(char *) &flag, sizeof(int));
	DIE(result < 0, "Setsockopt faild");

	//setare campuri structura pentru tcp
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd_tcp, (struct sockaddr *) &serv_addr, 
														sizeof(struct sockaddr));
	DIE(ret < 0, "bind function for tcp socket faild");
	//ascultare socket tcp passiv
	ret = listen(sockfd_tcp, MAX_CLIENTS);
	DIE(ret < 0, "listen function for tcp faild ");
	//actualizare fdmax
	fdmax = sockfd_tcp;
	//creare socket udp
	sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
	DIE(sockfd_udp < 0, "sockfd_udp");
	memset((char *) &cli_udp, 0, sizeof(cli_udp));
	//completare campuri structura pentru udp
	cli_udp.sin_family = AF_INET;
	cli_udp.sin_port = htons(portno);
	cli_udp.sin_addr.s_addr = INADDR_ANY;
	udplen = sizeof(cli_udp);

	int b = bind(sockfd_udp, (struct sockaddr *) &cli_udp, 
													sizeof(struct sockaddr));
	DIE(b < 0, "bind");
	//actualizare fdmax
	if (sockfd_udp > fdmax){
		fdmax = sockfd_udp;
	}
	// se adauga noii socketi in file descriptor 
	FD_SET(sockfd_tcp, &read_fds);
	FD_SET(sockfd_udp, &read_fds);
	FD_SET(0, &read_fds);

	while (1) {
		tmp_fds = read_fds; 
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		if (FD_ISSET(0, &tmp_fds))  { 
		//in cazul in care serverul primeste comanda exit
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN - 1, stdin);
			if (strncmp(buffer, "exit", 4) == 0) {
				for(int i = 1; i<=fdmax;i++) {
					if((i!=sockfd_tcp) &&(i!=sockfd_udp)){
					if (FD_ISSET(i, &read_fds)) {
						memset(buffer,0,BUFLEN);
						strcpy(buffer,"exit");
						//trimitere mesaj clienti tcp
						int m = send(i,buffer,BUFLEN,0);
						DIE(m < 0, "send");
						close(i);
					}
				}
				}
				break;
			} 
            else
                printf("Invalid command.\n");
        }
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				if (i == sockfd_udp) {
					//creare mesaj udp
				 	udp_msg msg_for_clients;
				 	memset(&msg_for_clients,0,sizeof( struct udp_message));
				 	memset(&buffer,0,BUFLEN);
				 	//primire mesaj udp
					int ret = recvfrom(sockfd_udp, &buffer, BUFLEN, 0, 
										(struct sockaddr*) &cli_udp, &udplen);
					DIE(ret < 0, "Nothing received from UDP socket.\n");
					memcpy(&msg_for_clients.ip,&cli_udp.sin_addr, 
														sizeof( struct in_addr));
					//completare mesaj udp transmis la sochetii tcp
					msg_for_clients.port = cli_udp.sin_port;
					memcpy(&msg_for_clients.topic,&buffer,50);
					msg_for_clients.data_type = buffer[50];
					memcpy(&msg_for_clients.payload,&buffer[51],1500);

					//parcurgere topicuri existente
					for(int k = 0; k < nr_topics; k++) {
						if(strcmp(map[k].topic,msg_for_clients.topic) == 0) {
							//parcurgere subscriberi topic cautat
							for(int j = 0; j< map[k].nr_subscribers; j++) {
								int ok = 0;
								//parcurgere clienti conectati
								for(int l = 0; l < nr_clients; l++) {
									if(strcmp(connected[l].id,
											map[k].subscribers[j].id) == 0) {
										int socket1 =  
											map[k].subscribers[j].client_socket;
											//trimitere mesaj
										int m = send(socket1,&msg_for_clients,sizeof(msg_for_clients),0);
										DIE(m < 0, "send");
										ok = 1;

									}

								}
								if(ok == 0) {
								//cazul in care clientul nu este conectat dar
									//este in lista de subscriberi
									int x = 0;
									if (map[k].subscribers[j].SF == 0)
										continue;
									//parcurgere mesaje deja salvate
									for(int l1 = 0; l1 < msg_saved;l1++ ) {
										//cazul in care un client mai are mesaje 
											//salvate 
										if(strcmp(save[l1].id , 
												map[k].subscribers[j].id)== 0) {
											struct udp_message *new;
											new = realloc(save[l1].msg,(save[l1].nr_msg +1) * sizeof(struct udp_message));
											if (new == NULL)
											{
						    					printf("realloc faild\n");
						    						
											}
											else {

												save[l1].msg = new;
												int nr = save[l1].nr_msg;
												memcpy(&save[l1].msg[nr],
													&msg_for_clients,sizeof(msg_for_clients));
												save[l1].nr_msg ++;
												x =1;
											}
										}
									}
									if(x == 0) {
										//cazul in care clientul nu mai are 
										//				mesaje salvate
										struct buffer_for_message aux;
										memset(&aux,0,
											sizeof(struct buffer_for_message));
										memcpy(&aux.id,&map[k].subscribers[j].id,
											sizeof(map[k].subscribers[j].id));
										aux.msg = malloc(sizeof(struct udp_message));
										memcpy(&aux.msg,&msg_for_clients,
														sizeof(msg_for_clients));
										aux.nr_msg = 1;
										save[msg_saved++] = aux;									
										}
								}

							}
							break;
						}
					}

				}
				else if (i == sockfd_tcp) {
					// acceptre cerere de conexiune pe socketul inactiv tcp
					memset(buffer, 0,BUFLEN);
					clilen = sizeof(cli_addr);
					new_client = accept(sockfd_tcp, 
										(struct sockaddr *) &cli_addr, &clilen);
					DIE(new_client < 0, "accept connection faild");
					n = recv(new_client, buffer, sizeof(buffer), 0);
					DIE(n < 0, "receve id message from client faild");

					int ok = 0;
					for(int j = 0; j< nr_clients; j++) {
						if(strcmp(connected[j].id,buffer) == 0) {
							char error[BUFLEN];
							sprintf(error, "error: This id is already used");
							int e = send(new_client, error, BUFLEN, 0);
							DIE(e < 0, "send");
							close(new_client);
							ok = 1;
							break;
						}
					}
					if(ok == 1)
						continue;
					printf("New client (%s) connected from %s:%d\n", buffer,
						inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


					//structura auxiliare pentru adaugarea clientului in 
						//vectorul de conectati
					struct client new_client_connected;
					memset(&new_client_connected,0,sizeof( struct client));
					new_client_connected.client_socket = new_client;
					memcpy(new_client_connected.id,buffer,strlen(buffer));
					new_client_connected.SF = 0;

					//realocare vector
					new_connected = realloc(connected,(nr_clients + 1) * 
														sizeof(struct client));
					if (new_connected == NULL)
					{
    					printf("realloc memory for new client faild\n");	
					}
					else {
						connected = new_connected;
					}
					connected[nr_clients++] = new_client_connected;
					//actulizare socket client in cazul in care a mai fost 
					//conectat anterior
					for(int j =0; j< nr_topics;j++) {
						for(int k =0;k<map[j].nr_subscribers;k++) {
							if(strcmp(map[j].subscribers[k].id,
												new_client_connected.id) == 0) {
									map[j].subscribers[k].client_socket = 
																	new_client;
								}
							}
						}
						//trimitere mesaje salvate
					for(int j = 0; j < msg_saved; j++) {
						if(strcmp(save[j].id,new_client_connected.id) == 0) {
							for(int l = 0; l < save[j].nr_msg ;l++) {
								int m = send(new_client,&save[j].msg[l],
													sizeof(save[j].msg[l]),0);
									DIE(m < 0, "send");
							}
							memset(&save[j].msg,0,sizeof(save[j].msg));
						}

						memmove(&save[j], &save[j+1], (msg_saved-j-1) * 
											sizeof(struct buffer_for_message));
						msg_saved--;
					}

					//adaugare socket in multimea descriptorilor
					FD_SET(new_client, &read_fds);
					//actualizare fdmax
					if (new_client > fdmax) { 
						fdmax = new_client;
					}
					//dezactivare algoirm Neagle
					flag = 1;
					result = setsockopt(new_client, IPPROTO_TCP, TCP_NODELAY, 
													(char *) &flag, sizeof(int));
 					DIE(result < 0, "setsockopt");
				} else {
					// s-au primit date de la unul din clienti
					// asa ca serverul trebuie sa le receptioneze
					tcp_msg msg ;
					memset(&msg,0,sizeof(msg));
					n = recv(i, &msg, sizeof(msg), 0);
					DIE(n < 0, "receve message from tcp client faild");
					if (n == 0) {
						// clientul a inchis conexiunea 
						for (int j = 0; j < nr_clients; j++)
						{
							if (connected[j].client_socket == i) {	
								//afisare mesaj deconectare
								printf("Client (%s) disconnected\n",
									connected[j].id);
								//eliminare vector clienti deconectati
								memmove(&connected[j], &connected[j+1], 
									(nr_clients-j-1) * sizeof(struct client));
								nr_clients--;
								//inchidere socket
								close(i);
							}
						}
						// se scoate din multimea de citire socketul inchis 
						FD_CLR(i, &read_fds);	
					} else {
						//se primeste mesaj de tipul subscribe
						if (msg.type == 1) { 
							struct client aux_client ;
							memset(&aux_client,0,sizeof(struct client));
							//parcurgere clienti conectati
							for (int j = 0; j< nr_clients;j++)
							{
								if(connected[j].client_socket == i) {
									aux_client = connected[j];		
								}
							}
							aux_client.SF = msg.SF;
							index = -1;
							new = false;
							//parcurgere topicuri
							for(int j = 0; j < nr_topics; j++)
							{
								new = false;	
								//cazul in care topicul exista deja
								if(strcmp(map[j].topic,msg.topic) == 0) {
									index = find_client(map[j].subscribers,
											map[j].nr_subscribers,aux_client.id);
									if (index == -1) {
										struct client *new_subscribers = NULL;
										new_subscribers = 
										realloc(map[j].subscribers,
											(map[j].nr_subscribers +1) * 
											sizeof(struct client));
										if (new_subscribers == NULL)
										{
											printf("realloc memory faild\n");
											
										}
										else {
											map[j].subscribers = new_subscribers;
											map[j].subscribers[map[j].nr_subscribers++] = aux_client;
										}
									}else {//cazul in care un client este deja 
										//abonat i se actualizeaza SF
										map[j].subscribers[index].SF = msg.SF;
									}
									break;
								}
								else {
									new = true;
								} 
							}
							//cazul in care nu exista topicul cautat sau
							//nu au mai fost date comenzi subscribe anterior
							if ((nr_topics == 0) || (new == true)) {
								struct Map *new_map = NULL;
								struct Map new_topic;
								memset(&new_topic,0,sizeof(struct Map));
								new_map = realloc(map,(nr_topics +1) * 
															sizeof(struct Map));
								if (new_map == NULL)
								{
									printf("realloc memory fore more topics faild\n");

								}
								else {
									map = new_map;
									memcpy(new_topic.topic,msg.topic,strlen(msg.topic));
									new_topic.subscribers = malloc(sizeof(struct client));
									new_topic.nr_subscribers = 1;
									new_topic.subscribers[0] = aux_client;		
									map[nr_topics++] = new_topic;
								}

							}
						}
						//cazul unui mesaj unsubscribed
						if (msg.type == 0) { 
							index = -1;
							new = false;
							//parcurgere topicuri
							for(int j = 0; j < nr_topics;j++)
							{
								if(strcmp(map[j].topic,msg.topic) == 0) {
									index = find_id(map[j].subscribers,
														map[j].nr_subscribers,i);
									//eliminare client de la topicul cautat
									if (index != -1){
										memmove(&map[j].subscribers[index], 
											&map[j].subscribers[index + 1], 
											(map[j].nr_subscribers-index-1) * 
											sizeof(struct client));
										map[j].nr_subscribers--;
									}
									if (map[j].nr_subscribers == 0) {
										memmove(&map[j], &map[j + 1], 
											(nr_topics-j-1) * sizeof(struct Map));
										nr_topics--;
									}
									new = true;
									break;

								} 
							}
							if (new == false) {			
								memset(buffer,0,BUFLEN);
								sprintf(buffer, "You are not subscribe");
								n = send(i, buffer, BUFLEN, 0);
								DIE(n < 0, "send unsubscribe error faild");
							}
						}

					}
				}
			}
		}
	}

	//inchidere socketi
	for(int i = 0; i<= fdmax; i++) {
		if (FD_ISSET(i, &read_fds)) {
			close(i);
		}
	}
	close(sockfd_tcp);
	free(connected);
	for(int j = 0; j < nr_topics; j++) {
		free(map[j].subscribers);
	}
	free(map);
	free(new_connected);
	return 0;
}