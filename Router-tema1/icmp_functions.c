/*Savu Ioana Rusalda 
		325CB		*/

/*FISIER CU FUNCTI DESTINATE PACHETELOR ICMP*/
#define IP_OFF (sizeof(struct ether_header))
#define ICMP_OFF (IP_OFF + sizeof(struct iphdr))

//functie completare hedere ethernet pentru mesaj de tip ICMP
void complete_ether_header(struct ether_header *eth_hdr_reply,
												struct ether_header *eth_hdr){
	memcpy(eth_hdr_reply->ether_dhost,eth_hdr->ether_shost,6);
	memcpy(eth_hdr_reply->ether_shost, eth_hdr->ether_dhost,6);
	eth_hdr_reply->ether_type = htons(ETHERTYPE_IP);
}
//functie completare header IP
void complete_ip_header(struct iphdr *ip_hdr_replay,int len,char* interface_ip,
														struct iphdr *ip_hdr){
	ip_hdr_replay->version = 4;
	ip_hdr_replay->ihl = 5;
	ip_hdr_replay->tos = 0;
	ip_hdr_replay->tot_len =  htons(len - sizeof(struct ether_header));
	ip_hdr_replay->id = htons(25);
	ip_hdr_replay->ttl = ip_hdr->ttl;
	ip_hdr_replay->protocol = IPPROTO_ICMP;
	ip_hdr_replay->check = 0;
	ip_hdr_replay->check = ip_checksum(ip_hdr_replay,sizeof(struct iphdr));			
	ip_hdr_replay->daddr = ip_hdr->saddr;
	ip_hdr_replay->saddr = inet_addr(interface_ip);
}
//functie completare header ICMP
void complete_icmp_header(struct icmphdr *icmp_hdr_replay,
											struct icmphdr *icmp_hdr,int type){
	icmp_hdr_replay->code = 0;
	icmp_hdr_replay->type = type;
	if(type == ICMP_ECHOREPLY){//pachet echo replay
		icmp_hdr_replay->un.echo.id = icmp_hdr->un.echo.id;
		icmp_hdr_replay->un.echo.sequence = icmp_hdr->un.echo.sequence;
	}
	else{//pachet time exedeed sau pachet destination unreachable
		icmp_hdr_replay->un.echo.id = 0;
		icmp_hdr_replay->un.echo.sequence = 0;
	}
	icmp_hdr_replay->checksum = 0;
	icmp_hdr_replay->checksum = ip_checksum(icmp_hdr_replay, sizeof(struct icmphdr)); 
}
//functie trimitere mesaj icmp
void send_icmp(int type, packet m){
	//declarare header ethernet pentru pachet primit
	struct ether_header *eth_hdr = (struct ether_header *)m.payload;
	//declarare header ip pentru pachet primit
	struct iphdr *ip_hdr = (struct iphdr *)(m.payload + IP_OFF);
	//declarare header icmp pentru pahet primit
	struct icmphdr *icmp_hdr = (struct icmphdr *)(m.payload +ICMP_OFF);	

	//calcularea ip-ului corespunzator interfetei pe care a fost primit mesajul
	char* interface_ip;			
	interface_ip = get_interface_ip(m.interface);

	packet reply;//creare de nou pachet
	//initializare si completare campuri pachet nou
	memset(reply.payload,0,sizeof(reply.payload));
	reply.len= sizeof(struct ether_header)+sizeof(struct iphdr)+
														sizeof(struct icmphdr);
	reply.interface = m.interface;

	//declararare si completare header ethernet pachet nou
	struct ether_header *eth_hdr_reply = (struct ether_header *)reply.payload;
	complete_ether_header(eth_hdr_reply,eth_hdr);

	//declararare si completare header ip pachet nou
	struct iphdr *ip_hdr_replay = (struct iphdr *)(reply.payload + IP_OFF);
	complete_ip_header(ip_hdr_replay,reply.len,interface_ip,ip_hdr);

	//declararare si completare header icmp pachet nou
	struct icmphdr *icmp_hdr_replay = (struct icmphdr *)(reply.payload + 
							sizeof(struct ether_header)+sizeof(struct iphdr));	
	complete_icmp_header(icmp_hdr_replay,icmp_hdr,type);	
	//trimitere pachet de tip icmp
	send_packet(m.interface,&reply);
}