/*
    Savu Ioana Rusalda 325CB
*/
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"

#define IP "3.8.116.10"
#define LEN 800

#include "requests.h"
#include "commands.c"


int main(int argc, char *argv[])
{
    int sockfd;
    char buf[50];
    char username[100];
    char password[100];
    char *message =  NULL;
    char *response = NULL;
    char url[100];
    //predefinire url
    strcpy(url,"/api/v1/tema/");
    char host [60];
    //predefinire host
    strcpy(host,"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com");
    char application[20];
    //predefinire aplicatie
    strcpy(application,"application/json");
    char *cookie =  NULL;
    int connected = 0;
    char string[100];
    char *token = NULL;

    //creare conexiune server
    sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
    while(1) {
        printf("Please enter command\n");
        scanf("%s", buf);//citire comanda tastatura
        if(strncmp(buf, "exit", 4) == 0) {
            break;
        }
        //comanda register
        if(strncmp(buf, "register", 8) == 0) {
            //citire tastatura username si parola
            printf("Please enter username & password\n");
            printf("username=");
            scanf("%s",username);
            printf("password=");
            scanf("%s",password);
            //compunere request
            register_command(username,password,&message,host,url,application);
            //trimitere mesaj la server
            puts(message);
            send_to_server(sockfd, message);
            //primire raspuns
            response = receive_from_server(sockfd);
            //verificare conexiune activa
            if(strlen(response) == 0) {
                //restabilire conexiune + retrimitere mesaj
                free(response);
                sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
            } 
            puts(response);
            printf("\n");
            free(response);
        }
        //comanda login
        if(strncmp(buf, "login", 5) == 0) {
            //verificare user deja conectat
            if(connected == 1) {
                printf("You are already login\n");
                continue;
            }
            //introducere username&password
            printf("Please enter username & password\n");
            printf("username=");
            scanf("%s",username);
            printf("password=");
            scanf("%s",password);
            //compunere request
            login_command(username,password,&message,host,url,application);
            //afisare mesaj request
            puts(message);
            printf("\n");
            //trimitere mesaj la server
            send_to_server(sockfd, message);
            free(message);
            //primire mesaj de la server
            response = receive_from_server(sockfd);
            //verificare conexiune activa
            if(strlen(response) == 0) {
                //restabilire conexiune+retrimitere mesaj
                free(response);
                sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
            } 
            //verificare succes mesaj
            if(strcmp(succes_response(response),"succes") != 0) {
                puts(response);
                printf("\n");
                free(response);
                continue;
            }else {
                connected = 1;
            }
            //afisare raspuns
            puts(response);
            printf("\n");
            free(cookie);
            //salvare cookie
            cookie  = get_cookie(response);
            free(response);
        }
        //comanda enter_library
        if (strncmp(buf, "enter_library", 13) == 0) {
            //verificare user conectat
            if (connected == 0) {
                printf("You are not authentificate\n");
                continue;
            }
            //compunere request
            entry_command(&message,cookie,host,url,application);
            //afisare mesaj trimis
            puts(message);
            printf("\n");
            //trimitere mesaj
            send_to_server(sockfd, message);
            //primire raspuns
            response = receive_from_server(sockfd);
            //verificare conexiune activa
            if(strlen(response) == 0) {
                //refacere conexiune + retrimitere mesaj
                free(response);
                sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                free(message);
                response = receive_from_server(sockfd);
            } 
            //verificare succes
            if(strcmp(succes_response(response),"succes") != 0) {
                puts(response);
                free(response);
                continue;
            }
            //afisare raspuns
            puts(response);
            printf("\n");
            free(token);
            //salvare token
            token = get_token(response);
            free(response);
            continue;    
        }
        //comanda add
        if (strncmp(buf, "add_book", 8) == 0) {
            //verificare user activ
            if(connected == 0) {
                printf("You are not authentificate\n");
                continue;
            }            
            //compunere request
            add_command(&message,token,host,url,application);
            //trimitere mesaj
            send_to_server(sockfd, message);
            //primire raspuns
            response = receive_from_server(sockfd);
            //verificare connexiune activa
            if(strlen(response) == 0) {
                free(response);
                //refacere conexiune + retrimitere mesaj
                sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                free(message);
                response = receive_from_server(sockfd);
            } 
            puts(response);
            printf("\n");
            free(response);
            continue;   
        }
        //comanda get_books
        if(strncmp(buf, "get_books", 9) == 0) {
            //verificare user conectat
            if(connected == 0) {
                printf("You are not authentificate\n");
                continue;
            } 
            //compunere mesaj trimis la server
            getBooks_command(&message,token,host,url,application);
            puts(message);
            //trimitere mesaj la server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //verificare conexiune activa
            if(strlen(response) == 0) {
                //restabilire conexiune + retrimitere mesaj
                free(response);
                sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
            } 
            //afisare mesaj
            puts(response);
            printf("\n");
            free(message);
            free(response);
            continue;
        
    }else {
        //comanda get_book
        if(strncmp(buf, "get_book", 8) == 0) {
            if(connected == 0) {
                printf("You are not authentificate\n");
                continue;
            }
            printf("Plese enter book id\n");
            scanf("%s",string);
            //compunere request
            getBook_command(&message,token,string,host,url,application);
            //afisare mesaj
            puts(message);
            //trimitere mesaj la server
            send_to_server(sockfd, message);
            //primire raspuns
            response = receive_from_server(sockfd);
            //verificare conexiune activa
            if(strlen(response) == 0) {
                //redeschidere conexiune + retrimitere mesaj
                free(response);
                sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
            }  
            //afisare mesaj                       
            puts(response);
            printf("\n");
            free(response);
            free(message);
            continue;

        } 
    }
    //comanda delete
    if (strncmp(buf, "delete_book", 11) == 0) {
        //verificare user conectat
        if(connected == 0) {
            printf("You are not authentificate\n");
            continue;
        }
        printf("Plese enter book id\n");
        scanf("%s",string);
        //compunere request
        delete_command(&message,token,string,host,url,application);
        puts(message);
        //trimitere mesaj la server
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
        //verificare conexiune activa
        if(strlen(response) == 0) {
            free(response);
            sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
        } 
        //afisare raspuns
        puts(response);
        printf("\n");
        free(message);
        free(response);
        continue;
           
    }
    //comanda login
    if (strncmp(buf, "logout", 6) == 0) {
        //verificare user conectat
        if(connected == 0) {
            printf("You are not authentificate\n");
            continue;
        }
        //compunere request
        logout_command(&message,cookie,host,url,application);
        puts(message);
        //trimitere mesaj server
        send_to_server(sockfd, message);
        //primire raspuns
        response = receive_from_server(sockfd);
        //verificare conexiune activa
        if(strlen(response) == 0) {
            //restabilire conexiune + retrimitere mesaj
            free(response);
            sockfd = open_connection(IP, 8080, AF_INET, SOCK_STREAM, 0);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
        } 
        //afisare raspuns
        puts(response);
        printf("\n");
        //verificare succes comanda
        if(strcmp(succes_response(response),"succes") == 0) {
            //marcarea faptului ca niciun user nu mai e conectat
            //eliberarea zonelor de memorie ce stocau cookie + token acces
            connected = 0;
            free(cookie);
            cookie =NULL;
            free(token);
            token = NULL;
        }
        free(response);
        free(message);
        continue;
    }             
    }

    free(cookie);
    free(token);
    close(sockfd);
    return 0;
}

























    //    free(message);
    // free(response);








        //         continue;
                   

        //     }
            

        // }


