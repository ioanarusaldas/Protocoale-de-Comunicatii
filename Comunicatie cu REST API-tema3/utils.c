/*
	Savu Ioana Rusalda 325CB
*/
#include "parson.c"
//extragere cookie din raspunsul serverului
char * get_cookie(char *response) {
	//cautare indice in inceput cookie in raspuns
  int pos1= strstr(response, "Set-Cookie") - response + 12;
  //cautare indice de final cookie in raspuns
  int pos2 = strstr(response,"Date") -response;
  char *start = &response[pos1];
  char *end = &response[pos2];
  //extragere + returnare cookie 
  char *substr = (char *)calloc(1, end - start + 1);
  memcpy(substr, start, end - start);
  return substr;

}
//functie extragere token autorizatie
char * get_token(char *response) {
	//cautare inceput cookie
	char* p = strstr(response, "{")  ;
	const char*t;
	//parsare token 
  	JSON_Value *parser =  json_parse_string(p);
  	JSON_Object * token = json_value_get_object (parser);
  	t = json_object_dotget_string (token, "token");
  	char *token_aux = (char *) malloc ( strlen(t)*sizeof(char));
    memcpy(token_aux,t,strlen(t));
  	json_value_free(parser);
  	return token_aux;
}
//verificare string ce poate fi reprezentat ca numar
bool isNumber (char* input) {

    for (int i = 0;i < strlen (input); i++) {
    	if((input[0] == '0') && (strlen(input)!=1)) {
    		return false;
    	}
        if (!isdigit(input[i]))
        {
            return false;
        }
    }
    return true;
}

//generare payload request de adaugare carte
void payload_msg (char **body_data,char **body_data_buffer) {
	//parsare json
	JSON_Value *root = json_value_init_object();
	JSON_Object *obj = json_value_get_object(root);
	char *string = NULL;
	json_object_set_string(obj,"title",body_data[0]);
	json_object_set_string(obj,"author",body_data[1]);
	json_object_set_string(obj,"genre",body_data[2]);
	//verificare string primit este reprezentat ca numar
	if(isNumber(body_data[3]) == false) {
		//trimitere string in foramt string pentru a primi eroare de la server
			//eroarea va fi afisata de catre client
		json_object_set_string(obj,"page_count",body_data[3]);
	} else {
		//trimitere string in format numeric
		json_object_set_number(obj,"page_count",atoi(body_data[3]));
	}
	json_object_set_string(obj,"publisher",body_data[4]);
	string = json_serialize_to_string_pretty(root);
	strcpy(*body_data_buffer,string);
 	json_free_serialized_string(string);
	json_value_free(root);
}
//payload mesaj register/login
void register_info (char **body_data,char **body_data_buffer) {
	//parsare username & password folosind json
	JSON_Value *root = json_value_init_object();
	JSON_Object *obj = json_value_get_object(root);
	char *string = NULL;
	json_object_set_string(obj,"username",body_data[0]);
	json_object_set_string(obj,"password",body_data[1]);
	string = json_serialize_to_string_pretty(root);
	strcpy(*body_data_buffer,string);
	json_free_serialized_string(string);
	json_value_free(root);
}