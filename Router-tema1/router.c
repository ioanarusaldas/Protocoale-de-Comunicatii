/*Savu Ioana Rusalda 
		325CB		*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include "net/if_arp.h"
#include "netinet/if_ether.h"
#include "skel.h"
#include "parser.h"//structura tabela arp + tabela rutare
#include "functii.c"// functii ajutatoare
#include "rtable_function.c"//functii aplicate tabelei de rutare
#include "arp_table_function.c"//functii aplicate tabelei arp
#include "queue.h"
#include "icmp_functions.c"//functii aplicate in crearea mesajelor ICMP

struct route_table_entry *rtable;//tabela rutare
int rtable_size;//dimensiune tabela rutare
struct arp_entry *arp_table;//tabela arp
int arp_table_len;//dimensiune tabela arp

//functie utilizata in actualizarea tabelei arp
void update_arptable(uint32_t ip, uint8_t mac[6]){
	//realocare dimensiune tabela arp
	arp_table = realloc(arp_table, sizeof(struct arp_entry)*(arp_table_len+1));
	//cazul in care tabela nu contine niciun element
	if(arp_table == NULL){
		arp_table_len = 1;//adaguarea primului element in tabela
		arp_table[arp_table_len - 1].ip = ip; 
		memcpy(arp_table[arp_table_len - 1].mac , mac, 6);
	}
	else{
		//verificare existenta intrare in tabela 
		struct arp_entry * best_arp = get_arp_entry(ip,arp_table,arp_table_len);
		if(best_arp == NULL){
			//adaugare element tabela in cazul in care acesta nu exista deja
			arp_table_len++;
			arp_table[arp_table_len - 1].ip = ip; 
			memcpy(arp_table[arp_table_len - 1].mac , mac, 6);
		}
	}
}
//functie de verificare corespondenta intre mesajul primit si pachetele ce 
													//se afla in asteptare
void verify_queue(queue wait,uint8_t arp_spa[4],uint8_t arp_mac[6]){
	packet * p ;//pachet auxiliar pentru extragerea din coada
	uint32_t ip;
	ip =  convert_string_ip(arp_spa);//convertire adresa ip
	queue aux;//coada auxiliare
	aux = queue_create ();
	
	while(!queue_empty(wait)){

		p = (packet*) queue_deq(wait);//extragere element din coada
		struct ether_header *eth_hdr= (struct ether_header *)p->payload;
		struct iphdr *ip_hdr = (struct iphdr *)(p->payload + 
												sizeof(struct ether_header));
		if(ntohl(ip) == ip_hdr->daddr){
			//verificare corespondenta mesaj primit si pachetele din coada
			update_arptable(ip_hdr->daddr,arp_mac);
			//actualizare mac destinatie
			memcpy(eth_hdr->ether_dhost,arp_mac,6);
			//trimitere pachet
			send_packet(p->interface,p);
		} 
		else{
			//intoducere elemente in coada auxiliara
			queue_enq(aux, p);
		}
	}
	//refacera cozii initiale
	while(!queue_empty(aux)){
		p = (packet*) queue_deq(aux);
		queue_enq(wait, p);
	}
}
int main(int argc, char *argv[])
{
	packet m;//pachet primit
	int rc;
	int file_of_size;//dimiensiune fisier 
	file_of_size = size_txt("rtable.txt");//numar linii fisiere
	rtable = malloc(sizeof(struct route_table_entry)*file_of_size);
	rtable_size = read_rtable(rtable);//parsare tabela de rutare

	//sortare tabela de rutare
	qsort(rtable, rtable_size, sizeof(struct route_table_entry), comparator); 
	
	arp_table = NULL;//initializare tabela arp
	arp_table_len =0;
	queue wait;//initializare coada de asteptare
	wait = queue_create ();

	init();
	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		char* interface_ip;//stocare interfata pe care a fost primit mesajul
		interface_ip = get_interface_ip(m.interface);

		//decalarare headere mesaj primit
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
		struct icmphdr *icmp_hdr = (struct icmphdr *)(m.payload + ICMP_OFF);

		//verificare mesaj de tip icmp
		if( (htons(eth_hdr -> ether_type) == ETHERTYPE_IP) &&
										(ip_hdr->protocol == IPPROTO_ICMP )){

			//verificare mesaj echo request adresat routerului
			if((icmp_hdr->type == ICMP_ECHO) && 
									(ip_hdr->daddr == inet_addr(interface_ip))){
				//trimitere pachet de tip icmp
				send_icmp(0,m);
				continue;
			}	
		}

		//verificare mesaj de tip ip
		if(htons(eth_hdr -> ether_type)  == ETHERTYPE_IP){
			if(ip_checksum(ip_hdr,sizeof(struct iphdr))!=0){
				//verificare mesaj corupt
				continue;
			}
			if(ip_hdr->ttl <= 1){
				//verificare time to live
				//trimitere mesaj echo-time-exceeded 
				send_icmp( ICMP_TIME_EXCEEDED ,m);
				continue;
			}

			//cautare destinatie in tabela de rutare
			struct route_table_entry *best_route =  
							get_best_route(ip_hdr->daddr,rtable,rtable_size);

			if(best_route == NULL){
				//verificare destinatie gasita
				//trimitere mesaj echo-destination-unreachable
				send_icmp(ICMP_DEST_UNREACH ,m);	
				continue;
			}
			//actualizare time to live + checksum
			ip_hdr->ttl--;
			ip_hdr->check = 0;
			ip_hdr->check = ip_checksum(ip_hdr,sizeof(struct iphdr));

			//cautare adresa mac destinatie
			struct arp_entry * arp= 
						get_arp_entry(ip_hdr->daddr,arp_table,arp_table_len);
			if(arp == NULL){
				//verificare gasire adresa mac destinatie
				//trimitere mesaj arp-request pentru aflarea adresei mac
				send_arp_request(m,best_route);
				packet copie;//creare pachet auxiliar
				copie.len = m.len;
				copie.interface = best_route->interface;
				memcpy(copie.payload,m.payload,sizeof(m.payload));
				//adaugare pachet in coada de asteptare
				queue_enq (wait,&copie);
				continue;
			}
			memcpy(eth_hdr->ether_dhost,arp->mac,6);
			get_interface_mac(best_route->interface,eth_hdr->ether_shost);
			send_packet(best_route->interface,&m);
		}

		//verificare mesaj de tip arp
		if(htons(eth_hdr->ether_type) == ETHERTYPE_ARP) {
			struct ether_arp * eth_arp = 
				(struct ether_arp *)(m.payload + sizeof(struct ether_header));
			//verificare mesaj de tip request
			if(ntohs(eth_arp->ea_hdr.ar_op) == ARPOP_REQUEST){
				//trimitere mesaj de tip replay
				send_arp_replay(m);
				continue;
			}
			//verificare mesaj de tip replay
			if(ntohs(eth_arp->ea_hdr.ar_op) == ARPOP_REPLY){	
		//verificare coada pentru a trimite din pachetele aflate in asteptare
				verify_queue(wait,eth_arp->arp_spa,eth_arp->arp_sha);	
				continue;
			}
		}
	}
	//eliberare memorie
	free(arp_table);
	free(rtable);
	return 0;
}


