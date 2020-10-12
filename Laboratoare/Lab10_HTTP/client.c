#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    // Ex 1.2: POST dummy and print response from main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/dummy", NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    // Ex 2: Login into main server
    int  body_data_fields_count1 = 1;
    char **body_data1 = (char **) malloc( body_data_fields_count1*sizeof(char*));
    body_data1[0] = (char *) malloc ( sizeof(char));
    sprintf(body_data1[0],"a=b");

    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/dummy","application/x-www-form-urlencoded",body_data1,  body_data_fields_count1,NULL,0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    // Ex 3: GET weather key from main server
    int body_data_fields_count = 2;
    char **body_data = (char **) malloc( body_data_fields_count*sizeof(char*));
    body_data[0] = (char *) malloc ( sizeof(char));
    strcpy(body_data[0],"username=student");
    body_data[1] = (char *) malloc ( sizeof(char));
    strcpy(body_data[1],"password=student");

    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/auth/login","application/x-www-form-urlencoded",body_data,  body_data_fields_count,NULL,0);

    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    // Ex 4: GET weather data from OpenWeather API
    int cookies_count = 1;
    char **cookies = (char **) malloc( cookies_count*sizeof(char*));
    cookies[0] = (char *) malloc (150* sizeof(char));
    strcpy(cookies[0],"connect.sid=s%3ADqKjz7gronsvfR7FVeXcOLCAlk6G4x-6.Vsn3DrLRI5JwJO7a9DVG1vnN6CSNNqEviF5hMhoPKcs; Path=/; HttpOnly");

    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/weather/key", NULL, cookies, cookies_count);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server
    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/auth/logout", NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    // BONUS: make the main server return "Already logged in!"


    // free the allocated data at the end!
     for(int i = 0; i < cookies_count; i++) {
        free(cookies[i]);
    }
     for(int i = 0; i < body_data_fields_count; i++) {
        free(body_data[i]);
     }

      for(int i = 0; i < body_data_fields_count1; i++) {
        free(body_data1[i]);
      }
    free(cookies);
    free(body_data);
    free(body_data1);
    close(sockfd);

    return 0;
}
