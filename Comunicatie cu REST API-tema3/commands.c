/*
	Savu Ioana Rusalda 325CB
*/
#define LEN 800
int add_command(char **message,char*token,char host [60],char url[100],
														char application[20]) {

	char url_aux[100];
	char *buffer = malloc(LEN*sizeof(char));
	size_t size = LEN;
	int body_data_fields_count = 6; 
	char **body_data = (char **) malloc( body_data_fields_count*sizeof(char*));

	//citire informatii despre carte
	getline(&buffer,&size,stdin);
	memset(buffer,0,LEN);
	printf("Please enter information about book\n");
	printf("title=");
	getline(&buffer,&size,stdin);
	body_data[0] = (char *) malloc (strlen(buffer)* sizeof(char));
	//prelucrare string
	memcpy(body_data[0],buffer,strlen(buffer));
	body_data[0][strlen(buffer) - 1] ='\0';
	memset(buffer,'\0',LEN);
	
	printf("author=");
	getline(&buffer,&size,stdin);
	body_data[1] = (char *) malloc (strlen(buffer)* sizeof(char));
	//prelucrare string
	memcpy(body_data[1],buffer,strlen(buffer));
	body_data[1][strlen(buffer) - 1] ='\0';
	memset(buffer,'\0',LEN);

	printf("genre=");
	getline(&buffer,&size,stdin);
	body_data[2] = (char *) malloc (strlen(buffer)* sizeof(char));
	//prelucrare string
	memcpy(body_data[2],buffer,strlen(buffer));
	body_data[2][strlen(buffer) - 1] ='\0';
	memset(buffer,'\0',LEN);

	printf("page_count=");
	getline(&buffer,&size,stdin);
	body_data[3] = (char *) malloc (strlen(buffer)* sizeof(char));
	//prelucrare string
	memcpy(body_data[3],buffer,strlen(buffer));
	body_data[3][strlen(buffer) - 1] ='\0';
	memset(buffer,'\0',LEN);

	printf("publisher=");
	getline(&buffer,&size,stdin);
	body_data[4] = (char *) malloc (strlen(buffer)* sizeof(char));
	//prelucrare string
	memcpy(body_data[4],buffer,strlen(buffer));
	body_data[4][strlen(buffer) - 1] ='\0';
	memset(buffer,'\0',LEN);	
	
	//adaugare token daca exista
	//daca token-ul nu exista acesta va fi NULL iar server-ul va intoarce eroare
	if(token != NULL) {
		body_data[5] = (char *) malloc ( strlen(token)*sizeof(char));
		sprintf(body_data[5],"%s",token);
	} else {
		body_data[5] = NULL;
	}
	//compunere url
	strcpy(url_aux,url);
	strcat(url_aux,"library/books");
	//compunere mesaj
	*message = compute_post_request(host,url_aux,application,body_data,
											body_data_fields_count,NULL,0,1);

	//eliberare
    for(int i = 0; i < body_data_fields_count; i++) {
        free(body_data[i]);
    }
    free(body_data);
    free(buffer);
    return 1;
}
//compunere mesaj pentru request de tip register
void register_command(char username[20],char password[20],char **message,
							char host [60],char url[100],char application[20]) {
	//alocare body_data
	char url_aux[100];
	int body_data_fields_count = 2;
    char **body_data = (char **) malloc( body_data_fields_count*sizeof(char*));
    body_data[0] = (char *) malloc ( sizeof(char));
    sprintf(body_data[0],"%s",username);
    body_data[1] = (char *) malloc ( sizeof(char));
    sprintf(body_data[1],"%s",password);
    //compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"auth/register");
    //compunere mesaj
    *message = compute_post_request(host, url_aux,application,body_data,
    											body_data_fields_count,NULL,0,0);
    //eliberare
    for(int i = 0; i < body_data_fields_count; i++) {
        free(body_data[i]);
    }
    free(body_data);
}
//compunere mesaj pentru request de tip login
void login_command(char username[20],char password[20],char **message,
							char host[60],char url[100],char application[20]) {
	char url_aux[100];
	//alocare body_data
	int body_data_fields_count = 2;
    char **body_data = (char **) malloc( body_data_fields_count*sizeof(char*));
    body_data[0] = (char *) malloc ( sizeof(char));
    sprintf(body_data[0],"%s",username);
    body_data[1] = (char *) malloc ( sizeof(char));
    sprintf(body_data[1],"%s",password);
    //compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"auth/login");
    //compunere mesaj
    *message = compute_post_request(host, url_aux,application,body_data,
    										body_data_fields_count,NULL,0,0);
    //eliberare
    for(int i = 0; i < body_data_fields_count; i++) {
        free(body_data[i]);
    }
    free(body_data);
}
//compunere mesaj pentru request de tip entry
void entry_command(char **message,char* cookie,char host [60],
										char url[100],char application[20]) {
	//alocare cookie
	char url_aux[100];
    int cookies_count = 1;
    char **cookies = (char **) malloc( cookies_count*sizeof(char*));
    cookies[0] = (char *) malloc (150* sizeof(char));
    strcpy(cookies[0],cookie);
    //compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"library/access");
    //compunere mesaj
    *message = compute_get_request(host,url_aux, cookies, cookies_count,NULL);
    //eliberare
    for(int i = 0; i < cookies_count; i++) {
        free(cookies[i]);
    }
    free(cookies);
}

//compunere request de tip getBooks
void getBooks_command(char **message, char* token,char host [60],char url[100],
														char application[20]) {

	char url_aux[100];
	//compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"library/books");
    //compunere mesaj
    *message = compute_get_request(host, url_aux,  NULL, 0,token);
}
//compunere request de tip getBook
void getBook_command(char **message,char* token,char string[100],char host [60],
											char url[100],char application[20]) {

	char url_aux[100];
	//compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"library/books/");
    strcat(url_aux,string);
    //compunere mesaj
    *message = compute_get_request(host, url_aux,NULL, 0,token);
}
//compunere request de tip delete
void delete_command(char **message, char* token,char string[100],char host [60],
											char url[100],char application[20]) {
	char url_aux[100];
	//compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"library/books/");
    strcat(url_aux,string);
    //compunere mesaj
    *message = compute_delete_request(host, url_aux, NULL, 0,token);
}
//compunere request de tip logout
void logout_command(char **message,char* cookie,char host [60],char url[100],
														char application[20]) {
	char url_aux[100];
	//alocare spatiu pentru cookie
	int cookies_count = 1;
	char **cookies = (char **) malloc( cookies_count*sizeof(char*));
	cookies[0] = (char *) malloc (150* sizeof(char));
	strcpy(cookies[0],cookie);
	//compunere url
    strcpy(url_aux,url);
    strcat(url_aux,"auth/logout");
    //compunere mesaj
	*message = compute_get_request(host, url_aux,cookies, cookies_count,NULL);
	//eliberare
    for(int i = 0; i < cookies_count; i++) {
        free(cookies[i]);
    }
    free(cookies);
}
//verificare succes mesaj
char* succes_response (char *response) {
	//calulare indice de inceput cod succes/eroare
	int pos1= strstr(response, "HTTP/1.1") - response + 9;
  	int pos2 = pos1 + 3;
  	char *start = &response[pos1];
  	char *end = &response[pos2];
  	//extragere substring ce corespunde codului de succes/eroare
  		//ex: 2xx, 3xx, 4xx, 5xx
  	char *substr = (char *)calloc(1, end - start + 1);
  	memcpy(substr, start, end - start);
  	//verificare succes
  	if ((atoi(substr)>= 200) && (atoi(substr)< 300)) {
  		free(substr);
  		return "succes";
  	}
  	//eliberare
  	free(substr);
  	return "";
}