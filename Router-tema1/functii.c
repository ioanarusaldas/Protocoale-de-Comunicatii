/*Savu Ioana Rusalda 
    325CB   */

/*FISIER DESTINAT FUNCTIILOR AJUTATOARE*/

int size_txt(char *file){
	int fd = open(file,O_RDONLY);
	if (fd < 0){
    	perror("Nu se poate deschide fisierul");
    	exit(1);
    }
    int size_of_file = lseek(fd, 0, SEEK_END); //dimensiune fisier
   	char buf [size_of_file];

   	lseek(fd,0,SEEK_SET);//pozitionare inceput de fisier
   	read(fd,buf,size_of_file);//citire continut fisier

   	int size = 0;
   	char * token;
    //calcul numar de linii din fisier
   	token = strtok(buf, "\n");//despartire continut dupa "\n"
    while (token != NULL) {
    	size++;
   		token = strtok(NULL, "\n");

    }
    close(fd);
    return size;
}

int max_mask(int index,struct route_table_entry *rtable ,uint32_t prefix){
  // alegere mask specific
  while(rtable[index].prefix == prefix ){
    index--;
  }
  return index +1;
}
int binarySearch_rtable(struct route_table_entry *rtable, int left ,int right, 
                                                              uint32_t address){ 
    if (right >= left) { 
      int mid = left + (right - left) / 2; 
        //verific daca elementul se afla pe pozitia din mijloc
      if ((rtable[mid].mask & address) == rtable[mid].prefix) {
          mid = max_mask(mid,rtable,rtable[mid].prefix);
          return mid; 
      }
      //daca elementul cautat este mai mic decat mijlocul se reface cautarea pe
          //subvectorul stang
      if ((rtable[mid].mask & address) <  rtable[mid].prefix) 
        return binarySearch_rtable(rtable, left, mid - 1, address); 
      //daca elementul cautat este mai mare decat mijlocul se reface cautarea pe
          //subvectorul drept
      return binarySearch_rtable(rtable, mid + 1, right, address); 
    } 
    return -1; 
}
void convert_num_ip(unsigned int ip, uint8_t *bytes)
{
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;   
}
uint32_t convert_string_ip(uint8_t arp_tpa[4]) {
  uint32_t ip = 0;
  int index = 3;
  for(int i = 0; i < 4; i++){
    if(i != 3){
      ip = ip + (arp_tpa[i] << (index*8));
    }
    else{
      ip = ip + arp_tpa[i];
    }
    index--;
  }
  return ip;
}
