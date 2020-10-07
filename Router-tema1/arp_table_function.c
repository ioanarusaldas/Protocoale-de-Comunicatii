/*Savu Ioana Rusalda 
    325CB   */

/*FISIER CU FUNCTII DESTINATE OPERATIILOR ASUPRA TABELEI ARP SI A SUPRA 
    MESAJELOR DE TIP ARP*/

//functie parsare tabela statica arp
int read_arptable(struct arp_entry *arptable){

	int fd = open("arp_table.txt",O_RDONLY);//deschidere fisier
	if (fd < 0){
    	perror("Nu se poate deschide fisierul");
    	exit(1);
  }
  int size_of_file = lseek(fd, 0, SEEK_END);//dimensiune fisier
  char buf [size_of_file];

  lseek(fd,0,SEEK_SET);
  read(fd, buf, sizeof(buf));//citire fisier

  char* token; 
  token = strtok(buf, " \n");

  int index = 1;//indice camp structura
  int elem = 0;//indice element 

  struct arp_entry arp ;//structura auxiliara
  while (token != NULL) {  	
    if(index == 1)
   	{
   		arp.ip =  inet_addr(token);//setare adresa ip
   		index++;
   	}
   	else{
   	  hwaddr_aton(token, arp.mac);//setare mac	
   		arptable[elem] = arp;//adaugare element tabela arp
   		elem++;
   		index = 1;
   	}
   	token = strtok(NULL, " \n");
  }
  close(fd);
	return elem;
}
//functie cautare adresa mac in tabela arp
struct arp_entry *get_arp_entry(__u32 ip,struct arp_entry *arp_table,
                                                            int arp_table_len) {
  for(int i = 0;i< arp_table_len;i++){
    if(arp_table[i].ip == ip){
      return &arp_table[i];
    }
  }   
  return NULL;
}
//functie completare arp-header pentru mesaj de tip request
void complete_arp_header_request(struct ether_arp * arp_request,packet request,
                                      char* interface_ip,struct iphdr *ip_hdr ){
  arp_request->ea_hdr.ar_hrd = htons(1);
  arp_request->ea_hdr.ar_pro = ntohs(2048);
  arp_request->ea_hdr.ar_hln= 6;
  arp_request->ea_hdr.ar_pln = 4;
  arp_request->ea_hdr.ar_op = htons(ARPOP_REQUEST);

  get_interface_mac(request.interface,arp_request->arp_sha);
  //convertire ip string-vector
  sscanf(interface_ip,"%hhu.%hhu.%hhu.%hhu",&arp_request->arp_spa[0],
    &arp_request->arp_spa[1],&arp_request->arp_spa[2],&arp_request->arp_spa[3]);
  for(int i=0; i<ETH_ALEN;i++){
    arp_request->arp_tha[i] = 0;
  }
  //convertire adresa ip 
  convert_num_ip(ip_hdr->daddr,arp_request->arp_tpa);
}
//functie completare header ethernet pentru mesaj de tip request
void complete_eth_header_request(struct ether_header *eth_hdr_request,packet 
                                                                      request){
  eth_hdr_request->ether_type = htons(ETHERTYPE_ARP);
  for(int i = 0; i < 6; i++){
    eth_hdr_request->ether_dhost[i] = 255;    
  }
  get_interface_mac(request.interface,eth_hdr_request->ether_shost);
}
//functie completare header ethernet pentru mesaj de tip reply
void complete_eth_header_reply(struct ether_header *eth_hdr_reply,
                                        struct ether_header *eth_hdr,packet m){
  memcpy(eth_hdr_reply->ether_dhost, eth_hdr->ether_shost,6);
  get_interface_mac(m.interface,eth_hdr_reply->ether_shost);
  eth_hdr_reply->ether_type = htons(ETHERTYPE_ARP);
}
//functie completare header arp pentru mesaj de tip reply
void complete_arp_header_reply(struct ether_arp * arp_reply,
                          struct ether_arp * arp,char* interface_ip,packet m ){
  arp_reply->ea_hdr.ar_hrd = htons(1);
  arp_reply->ea_hdr.ar_pro = ntohs(2048);
  arp_reply->ea_hdr.ar_hln= 6;
  arp_reply->ea_hdr.ar_pln = 4;
  arp_reply->ea_hdr.ar_op = htons(ARPOP_REPLY);
  memcpy(arp_reply->arp_tha,arp->arp_sha,6);
  memcpy(arp_reply->arp_tpa,arp->arp_spa,4);
  //convertire adresa ip string-vector
  sscanf(interface_ip,"%hhu.%hhu.%hhu.%hhu",&arp_reply->arp_spa[0],
          &arp_reply->arp_spa[1],&arp_reply->arp_spa[2],&arp_reply->arp_spa[3]);
  get_interface_mac(m.interface,arp_reply->arp_sha);
}
//functie trimitere pachet de tip arp request
void send_arp_request( packet m,struct route_table_entry *best_route){
  //header ip pachet primit
  struct iphdr *ip_hdr = (struct iphdr *)(m.payload + 
                                                  sizeof(struct ether_header));
  char* interface_ip;   
  //stocare interfata pe care a fost primit pachetul  
  interface_ip = get_interface_ip(m.interface);
  packet request;//creare pachet arp_request

  //declarare header ethernet pachet nou
  struct ether_header *eth_hdr_request = (struct ether_header *)request.payload;
  //declarare header arp pachet nou
  struct ether_arp * arp_request = (struct ether_arp *)(request.payload + 
                                                  sizeof(struct ether_header));
  
  //initializare si setare campuri structura packet
  memset(request.payload,0,sizeof(request.payload));
  request.len= sizeof(struct ether_header)+ sizeof(struct ether_arp);
  //setare interfata unde trebuie trimis pachetul
  request.interface =  best_route -> interface;

  //completare headere
  complete_eth_header_request(eth_hdr_request,request);
  complete_arp_header_request(arp_request,request,interface_ip,ip_hdr);
  //trimitere pachet de tip arp_request
  send_packet(request.interface,&request);
}
//functie trimitere pachet de tip arp -reply
void send_arp_replay( packet m){
  //accesare headere pachet primit
  struct ether_header *eth_hdr = (struct ether_header *)m.payload;
  struct ether_arp * arp= (struct ether_arp *)(m.payload + 
                                                  sizeof(struct ether_header));
  //salvare interfata pe care a fost primit pachetul
  char* interface_ip;     
  interface_ip = get_interface_ip(m.interface);

  packet reply;//creare pachet nou
  //declarare hedere pachet nou
  struct ether_header *eth_hdr_reply = (struct ether_header *)reply.payload;
  struct ether_arp * arp_reply= (struct ether_arp *)(reply.payload + 
                                                  sizeof(struct ether_header));
  //initializare si setare campuri structura packet
  memset(reply.payload,0,sizeof(reply.payload));
  reply.len= sizeof(struct ether_header)+ sizeof(struct ether_arp);
  reply.interface = m.interface;

  //completare headere
  complete_eth_header_reply(eth_hdr_reply,eth_hdr,m);
  complete_arp_header_reply(arp_reply,arp,interface_ip, m );
  //trimitere pachet de tip arp reply
  send_packet(m.interface,&reply);
}




