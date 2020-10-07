//Savu Ioana Rusalda 325CB
//cautare client dupa id
int find_client (struct client * subscribers, int nr_subscribers,char id[10]){
	for (int i = 0; i < nr_subscribers; i++) {
		if (strcmp(subscribers[i].id, id) == 0)
			return i;
	}
	return -1;
}
//cautare client dupa socket
int find_id (struct client * subscribers, int nr_subscribers,int client_socket){

 	for (int i = 0; i < nr_subscribers; i++) {
		if (subscribers[i].client_socket == client_socket)
			return i;
	}
	return -1 ;
}
//trimitere mesaje clienti conectati
void send_udp_connected (struct Map *map ,int nr_topics,udp_msg msg_for_clients,
									int nr_clients,struct client * connected) {

	int ret = 0;
	for (int j = 0; j < nr_topics; j++) {
		if (strcmp(msg_for_clients.topic,map[j].topic) == 0) {
			for (int k = 0; k < map[j].nr_subscribers; k++) {
				for (int i = 0; i < nr_clients; i++) {
					if (strcmp( map[j].subscribers[k].id,connected[i].id ) == 0) {
						ret = send(map[j].subscribers[k].client_socket,
									&msg_for_clients,sizeof(msg_for_clients),0);
						DIE(ret < 0, "send");
					}
				}
			}
			break;
		}
	}

}
// void save_message(struct udp_msg msg,struct buffer_for_message *buffer) {
// 	if (buffer->nr_msg == buffer->capacity) {
// 		buffer->capacity *= 2;
// 		struct buffer_for_message aux;
// 		aux = realloc(buffer->msg,(buffer->capacity) * sizeof(udp_msg));
// 		if (aux == NULL) {
// 			printf("realloc faild for saving message\n");
// 		} else {
// 			buffer->msg = aux;

// 		}
// 	}
// 	memcpy(&buffer->msg,&msg,sizeof(msg));
// 	buffer->nr_msg++;

// }

// int find_buffer (struct buffer_for_message * buffer , char[10] id ) {
// 	int size  = sizeof(buffer) / sizeof(struct buffer_for_message);
// 	for (int i = 0; i <= size; i++) {
// 		if (strcmp(buffer[i].id,id) == 0) {
// 			return i;
// 		}
// 	}
// 	return -1;

// }


// void store_udp_disconnected (struct Map *map ,int nr_topics,udp_msg 
//msg_for_clients,int nr_clients,struct client * disconnected,struct udp_msg msg
//,struct buffer_for_message** save) {
// 	int ret = 0;
// 	for (int j = 0; j < nr_topics; j++) {
// 		if (strcmp(msg_for_clients.topic,map[j].topic) == 0) {
// 			for (int k = 0; k < map[j].nr_subscribers; k++) {
// 				for (int i = 0; i < nr_clients; i++) {
// 					if (strcmp( map[j].subscribers[k].id,disconnected[i].id ) == 0) {
// 						printf("saving message for client %s\n", disconnected[i].id );
// 						int index = find_buffer(buffer,disconnected[i].id);
// 						if (index == -1) {

// 						}
// 						save_message(msg,&save[index]);
// 					}
// 				}
// 			}
// 			break;
// 		}
// 	}

// }

