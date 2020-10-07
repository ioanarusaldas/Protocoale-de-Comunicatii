/*
    Savu Ioana Rusalda 325CB
*/
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#include "utils.c"

//compunere mesaj de tip get_request
char *compute_get_request(char *host, char *url,
                            char **cookies, int cookies_count,char *token)
{
    //alocare mesaj
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    //compunere headere
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "HOST: %s:8080", host);
    compute_message(message, line);
    //adaugare cookie
    if (cookies != NULL) {//verificare existenta cookie
        for(int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s",cookies[i]);
            compute_message(message, line);
        }
    }
    //adaugare token
    if (token != NULL) {//verificare existenta token
        sprintf(line, "Authorization: Bearer %s",token);
        compute_message(message, line);
    }
    //adaugare linie goala
    compute_message(message, "");
    free(line);
    return message;
}

//compunere mesaj de tip post_request
char *compute_post_request(char *host, char *url, char* content_type, 
                            char **body_data,int body_data_fields_count, 
                            char **cookies, int cookies_count, int type)
{
    //alocare mesaj
    //alocare continut mesaj
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    //adaugare headere
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "HOST: %s:8080", host);
    compute_message(message, line);
    /* 1-post_request cu verificarea autentificarii
        0-post_request de tip inregistrare
    */
    if(type == 1) {//verificare cerere de tip post
        if(body_data[5] != NULL) {
            sprintf(line, "Authorization: Bearer %s", body_data[5]);
            compute_message(message, line);
        }    
    }
    sprintf(line, "Content-Type: %s",content_type);
    compute_message(message, line);

    int length = 0;
    //augare date 
    if(body_data_fields_count <= 2) {//date de tip username&password
        register_info(body_data,&body_data_buffer);
    } else {
        payload_msg(body_data,&body_data_buffer);//date de tip informatii carte   
    }
    length = strlen(body_data_buffer);//calculare lungime date

    sprintf(line, "Content-Length: %d",length );
    compute_message(message, line);

    //adaugare cookie
    if (cookies != NULL) {//verificare existenta cookie
        for(int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s",cookies[i]);
            compute_message(message, line);
        }
    }
    //adaugare linie goala
    compute_message(message, "");

    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}
//compunere mesaj de tip delete_request
char *compute_delete_request(char *host, char *url,
                            char **cookies, int cookies_count,char *token)
{
    //alocare mesaj
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    //setare headere
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "HOST: %s:8080", host);
    compute_message(message, line);

    //adaugare cookie
    if (cookies != NULL) {//verificare existenta cookie
        for(int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s",cookies[i]);
            compute_message(message, line);
        }
    }
    //adaugare token autorizatie
    if (token != NULL) {//verificare existenta token
        sprintf(line, "Authorization: Bearer %s",token);
        compute_message(message, line);
    }

    compute_message(message, "");
    free(line);
    return message;
}
