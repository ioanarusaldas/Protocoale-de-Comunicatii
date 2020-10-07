#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url,
							char **cookies, int cookies_count,char *token);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count,int type);
char *compute_delete_request(char *host, char *url,
                            char **cookies, int cookies_count,char *token);
char * get_cookie(char *response);
char * get_token(char *response) ;
void payload_msg (char **body_data,char **body_data_buffer);

#endif
