/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "helpers.h"


void usage(char*file)
{
	fprintf(stderr,"Usage: %s server_port file\n",file);
	exit(0);
}

/*
*	Utilizare: ./server server_port nume_fisier
*/
int main(int argc,char**argv)
{
	int fd;

	if (argc!=3)
		usage(argv[0]);
	
	struct sockaddr_in my_sockaddr, from_station ;
	char buf[BUFLEN];


	/*Deschidere socket*/
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s < 0)
		perror("Socket creation failed");

	
	
	/*Setare struct sockaddr_in pentru a asculta pe portul respectiv */

	
	/* Legare proprietati de socket */
	struct sockaddr_in in;
	in.sin_family = AF_INET;
	in.sin_port = htons(12345);
	in.sin_addr.s_addr = INADDR_ANY;

	int b = bind(s, (struct sockaddr*) &in, sizeof(in));
	if(b < 0)
		perror("Socket creation failed");
	struct sockaddr_in client_info;
	socklen_t length = sizeof(client_info);




	
	
	/* Deschidere fisier pentru scriere */
	DIE((fd=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644))==-1,"open file");

	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din socket
	*		pune in fisier

	*/
	
	int r;

	while((r = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*) &client_info, &length))>0){
		write(fd,buf,r);

		if (r < 0){
			break;
    		perror("Socket creation failed");
		}


	}
	


	/*Inchidere socket*/	
	close(fd);
	close(s);

	
	/*Inchidere fisier*/

	return 0;
}
