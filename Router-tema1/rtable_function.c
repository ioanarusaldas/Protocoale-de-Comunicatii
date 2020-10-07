/*Savu Ioana Rusalda 
    325CB   */

/*FISIER DESTINAT FUNTIILOR CARE OPEREAZA ASUPRA TABELEI DE RUTARE*/

int read_rtable(struct route_table_entry *rtable){
	int fd = open("rtable.txt",O_RDONLY);//deschidere fisier
	if (fd < 0){
    	perror("Nu se poate deschide fisierul");
    	exit(1);
    }
  
    int size_of_file = lseek(fd, 0, SEEK_END);//calul dimensiune fisier
   	char buf [size_of_file];

   	lseek(fd,0,SEEK_SET);//pozitionare inceput fisier
   	read(fd, buf, sizeof(buf));//citire continut fisier

    char* token; 
    struct route_table_entry rentry ;//creare structura auxiliara

  	token = strtok(buf, " \n");//despartire fisier dupa spatiu si "\n"
  	int index = 1;//indice element structura
  	int elem = 1;//numar elemente rtable
    while (token != NULL) {
    	if(index < 4)
   		{
        switch (index)
        {//setare elemente structura
          case 1: {
            rentry.prefix = inet_addr(token);
            break;}
          case 2: {
            rentry.next_hop = inet_addr(token);
            break;
          }
          case 3: {
            rentry.mask = inet_addr(token);;
            break;
          }
          default: break;
        }
        index++;
   	  }
   		else{

   			rentry.interface = atoi(token);
   			rtable[elem-1] = rentry;//adagugare element 
   			elem++;
   			index = 1;
   		}
   		token = strtok(NULL, " \n");
    }
    close(fd);
	return elem-1;//numar elemente rtable
}

int comparator(const void *addr1, const void *addr2) 
{ //functie de comparatie folosita in cautarea binara
    struct route_table_entry a = *(struct route_table_entry *)addr1; 
    struct route_table_entry b = *(struct route_table_entry *)addr2; 
    if(a.prefix == b.prefix)//in caz de egalitate dupa prefix sortez dupa mask
    {
      if(a.mask < b.mask)
        return 1;
      else return -1;
 
    }
    else{
      if(a.prefix > b.prefix)
        return 1;
      else return -1;
    }
    return 0;   
}
//functie afisare rtable folosita la debug
void print(struct route_table_entry *rtable, int size){
  for(int i = 0; i < size ; i++){
    printf("%u %u %u %d\n",rtable[i].prefix,rtable[i].next_hop,
                                          rtable[i].mask,rtable[i].interface );
  }
}
//pasul in care se face match pe tabela de rutare pentr a gasi destinatia
struct route_table_entry *get_best_route(__u32 dest_ip,
                                  struct route_table_entry *rtable, int size){
  //cautare ip destinatie in tabela de rutare + masca cea mai specifica
  int result = binarySearch_rtable(rtable, 0, size - 1,dest_ip);
  if(result != -1){
    return &rtable[result];//returnare rezultat din tabela 
  }
  return NULL;
}  