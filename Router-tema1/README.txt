/*Savu Ioana Rusalda 
		325CB		*/

Punctaj checker local: TOTAL: 110/110
Implementare: Tabela arp dinamica + cautare cu complexitate ceruta(cautare binara + sortare
tabela cu qsort)

ROUTER.C

Fisierul contine:
	-functie actualizare tabela arp (update_arptable)
	-functie trimitere pachete aflate in coada de asteptare(verify_queue)
	-functia main

	*Functia UPDATE_ARPTABLE 
	In cadrul acestei functii este realocata tabela arp in functie de numarul de
elemente. In cazul in care tabela nu contine niciun element este adaugat primul, altfel
se verifica daca tabela contine deja elemntul ce urmeaza a fi introdus pentru a nu 
se crea duplicate.
	**Functia VERIFY_QUEUE
	In cadrul acestei functii iterez prin fiecare element din coada de asteptare si
trimit toate pachetele care se protivesc mesajului arp_replay care a sosit. Inainte de 
atrimite pachetul actualizez adresa MAC destinatie cu cea primita la arp_reply.
	***Functia MAIN
	In cadrul functiei main verific tipul pachetului primit(mesaj ip sau arp).
De asemenea, verific si daca pachetul este de tip ICMP. IN cazul unui pachet de tip ICMP
verific ca acesta sa fie un ECHO REQUEST si sa fie adresat router-ului.
Daca este un mesaj de tip IP verific sa nu fie corupt sau sa ca TTL sa nu fie <= 1.Daca nu este 
accesat niciunul dintre cazuri caut in tabela de rutare destinatia si apoi adresa mac a 
destinatiei in tabela arp.
Daca nu este gasita o adresa mac corespunzatoare trimit un arp request si adaug pacgetul 
in COADA de asteptare.
In cadrul unui mesaj de tip ARP verific daca este arp request sau arp replay. Daca este un 
mesaj de arp request trimit un arp REPLY, iar daca este un mesaj de tip replay actualizez
tabela de rutare si trimit pachetelele din coada care corespund mesajului primit.


ICMP_FUNCTIONS.C
	
Fisierul contine:
	-functie completare header ethernet pentru pachet de tip icmp(complete_ether_header)
	-functie completare header ip pentru pachet de tip icmp(complete_ip_header)
	-functie completare header icmp pentru pachet de tip icmp(complete_icmp_header)
	-functie de creare si trimitere a pachetelor de tip icmp(send_icmp)

	*Functia COMPLETE_ETHER_HEADER
	In cadrul acestei functii setez la nivelul headerului de ethernet adresa destinatie
a noului pachet cu adresa sursa a pachetului primit pentru a intoarce un mesaj de 
raspuns de timp ICMP.Adresa sursa va fi setata cu adresa destinatie a pachetului primit.
	**Functia COMPLETE_IP_HEADER
	In cadrul acestei functii completez campurile structurii ip conform laboratorului 4.
Setez adresa destinatie cu ce a sursei iar adresa sursa cu adresa ip pe care o aflu folosind
functia get_interface_ip pe interfata pachetului primit.
	***Functia ICMP_HEADER 
	In cadrul acestei functii completez campurile structurii icmp conform laboratorului 5.
Daca mesajul ce urmeaza a fi trimis este de tip ecorequest type va fi 
setat pe 0(ICMP_ECHOREPLY) iar id-ul sisequence vor fi preluate din vechiul mesaj.
Daca mesajul ce urmeaza a fi trimis este de tip time-exeeded type va fi completat 
cu 11(ICMP_TIME_EXEEDED), id si sequence fiind pe 0.
Daca mesajul ce urmeaza a fi trimis este de  tip destination-unreachable type va fi
setat pe 3(ICMP_DEST_UNREACH) iar id si sequence pe 0.
	****Functia SEND_ICMP
	In cadrul acestei functii creez un nou pachet la care ma folosesc de functiile
anterioare pentru a completa headerele de ethernet,ip si icmp.Aceasta functie primeste ca parametru 
tipul mesajului pentru a sti ce tip de mesaj icmp trebuie trimis.
Dupa crearea noului pachet, acesta este trimis pe interfata prin care a fost primit mesajul 
anterior.

ARP_TABLE_FUNCTION.C

Fisierul contine:

 	-functie parsare tabela arp statica(nefolosita in acest stadiu al programului)
	-functie de cautare a adrei mac in tabela arp(get_arp_entry)
	-functie completare header ethernet pentru mesaj de tip request(complete_eth_header_request)
	-functie completare arp-header pentru mesaj de tip request(complete_arp_header_request)
	-functie completare header ethernet pentru mesaj de tip reply(complete_eth_ether_reply)
	-functie completare header arp pentru mesaj de tip reply(complete_arp_header_reply)
	-functie trimitere pachet de tip arp request(send_arp_request)
	-functie trimitere pachet de tip arp -reply(send_arp_reply)

	*Functiile COMPLETE_ETH_HEADER_REQUEST/REPLY
	In cadrul acestor functii am realizat completarea headerelor de ethernet pentru cele 
2 tipuri de mesaj.La mesajului request am setat destinatia cu ff:ff:ff:ff:ff:ff iar 
sursa cu adresa mac a interfetei pe care urmeaza sa fie trimis pachetul.La mesajul de tip
reply am completat adresa destinatie cu adresa sursa a mesajului primit iar adresa sursa cu
adresa mac a interfetei pe care a venit pachetul.
	**Functiile COMPLETE_ARP_HEADER_REQUEST/REPLY
	In cadrul acestor functii am completat headerelor arp destinate celor 2 tipuri de pachete. 
Pentru a face aceasta, m-am uitat cu programul Wireshark la un mesaj de acest timp,
cu ajutorul tabelei statice.
	***Functia GET_ARP_ENTRY
	In cadrul acestei caut in tabela arp adresa mac corespunzatoare unui anumit ip.
	****Functiile SEND_ARP_REQUEST/REPLY
	In cadrul acestei functii creez un mesaj de tip arp request,respectiv arp reply
ajutandu-ma de functiile mentionate anterior.

RTABLE_FUNCTION.C

Fisierul contine:

	-functie parsare tabela de rutare(read_rtable)
	-functie de comparatie pentru sortarea tabelei de rutare(comparator)
	-functie afisare tabela de rutare nefolosit in acest stadiu al probramului(print)
	-functie cautare in tabela de rutare(get_best_route)

	*Functia READ_RTABLE
	In cadrul acestei functii construiesc tabela de rutare pe baza informatiilor citite din fisier.
	**Functia COMPARATOR
	Aceasta functie compara 2 elemente din tabela de rutare dupa prefix si in caz de egalitate dupa masca.
Sortarea se realizeaza cu Qsort
	***Functia GET_BEST_ROUTE
	In cadrul acestei functii ma folosesc de cautarea binara pentru a gasi destinatia pachetului.


FUNCTII.c

Fisierul contine:
	-functie de calculare elemente tabela rutare(size_txt)
	-functie calculare masca cea mai specifica(max_mask)
	-functie cautare binara (binarySearch_rtable)
	-functie conversie numar intreg in vector cu elemente uint8_t(convert_num_ip)
	-functie conversie string in vector cu elemente uint8_t(convert_string_ip)

	*Functia SIZE_TXT
	In aceasta functie citesc din fisier si numar liniile din acesta.Numarul de linii va
corespunde numarului de elemente din tabela de rutare
	**Functia MAX_SIZE
	In aceasta functie parcurg secventa de adrese cu acelasi ip pentru a ajunge la prima masca,
tabela fiind sortata dupa prefix si masca.
	***Functia binarySerach_rtable 
	Algoritmul pentru aceasta functie a fost luat de pe internet si modificat pentru
a realiza cautarea in tabela de rutare dupa prefix si pentru a returna cu ajutorul
functiei MAX_MASK adresa corecta.












































