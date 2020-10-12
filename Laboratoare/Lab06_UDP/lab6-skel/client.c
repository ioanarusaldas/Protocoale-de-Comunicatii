/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
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
	fprintf(stderr,"Usage: %s ip_server port_server file\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=4)
		usage(argv[0]);
	
	int fd;
	struct sockaddr_in to_station;
	char buf[BUFLEN];


	/*Deschidere socket*/
	
	int s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s < 0)
    	perror("Socket creation failed");
    struct sockaddr_in in;
	in.sin_family = AF_INET;
	in.sin_port = htons(12345);

	inet_aton("127.0.0.1", &in.sin_addr);



	
	/* Deschidere fisier pentru citire */
	DIE((fd=open(argv[3],O_RDONLY))==-1,"open file");
	
	/*Setare struct sockaddr_in pentru a specifica unde trimit datele*/
	int n;
	while((n = read(fd,buf,BUFLEN))>0){

		int p = sendto(s, buf,n, 0, (struct sockaddr*) &in, sizeof(in));
		if (p < 0){
			break;
    		perror("Socket creation failed");
		}

	}
	
	
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din fisier
	*		trimite pe socket
	*/	


	/*Inchidere socket*/
	close(s);
	close(fd);

	
	/*Inchidere fisier*/

	return 0;
}
