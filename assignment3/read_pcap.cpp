#include <iostream>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <pcap/pcap.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <time.h>
#include <cstdio>
#include "include/CSVWriter.h"
#include <netinet/ether.h>
#include <string>
using namespace std;

// UDP header struct
struct UDP_hdr {
    u_short uh_sport;   // source port
    u_short uh_dport;   // destination port
    u_short uh_ulen;    // datagram length
    u_short uh_sum; // datagram checksum
};

CSVWriter csv;


string printAddr(u_char *ptr){
    string mac;
    char hexString[20];
    for(int i= 0; i < ETHER_ADDR_LEN;i++){
        sprintf(hexString, "%x", ptr[i]);
        mac += hexString;
        if(i!= ETHER_ADDR_LEN -1){
            mac+=":";
        }
    }
    //printf("\n");
    return mac;
}

void parsePacket(const unsigned char *packet, struct timeval ts, const struct pcap_pkthdr *pkthdr) {

    struct ip *ip;
    unsigned int ipHeaderLength;
    struct tcphdr *tcpHeader;
    u_int sourcePort,destPort;
    struct ether_header *ethernetHeader;
    char sourceIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];
    time_t local_tv_sec;
    char buff[256];
    string sourceMAC,destMAC;

    strftime(buff,20,"%Y-%m-%d %H:%M:%S", localtime(&ts.tv_sec));
    csv.newRow() << buff;

    // if the packet is IP packet -> print out the source and destination ip
    ethernetHeader=(struct ether_header *)packet;
    packet+=sizeof(struct ether_header);
    sourceMAC=printAddr(ethernetHeader->ether_shost);
    destMAC=printAddr(ethernetHeader->ether_dhost);


    if(ntohs(ethernetHeader->ether_type)==ETHERTYPE_IP) {
        ip=(struct ip *)packet; 
        inet_ntop(AF_INET,&(ip->ip_src),sourceIp,INET_ADDRSTRLEN);
        inet_ntop(AF_INET,&(ip->ip_dst),destIp,INET_ADDRSTRLEN);
        //cout << sourceIp << " " << destIp << " " << " ";
        if(ip->ip_p==IPPROTO_TCP) {
        tcpHeader=(tcphdr *)(packet+sizeof(struct ip));
        sourcePort = ntohs(tcpHeader->source);
        destPort = ntohs(tcpHeader->dest);
        //cout << sourcePort << " " << destPort << endl;
        csv<< sourceMAC << sourceIp << sourcePort << destMAC << destIp << destPort << "tcp";
        }
    }
}



int main(int argc, char *argv[]) {

    pcap_t *pcap;
    char errbuf[PCAP_ERRBUF_SIZE];
    const unsigned char *packet;
    struct pcap_pkthdr header;

    csv.newRow() << "Timestamp" << "Sender MAC" << "Sender IP" << "Sender Port" << "Target MAC" << "Target IP" << "Target Port" << "Transport Protocol";

    if(argc!=3) {
        cout << "ERROR: fewer parameters provided" << endl;
        exit(1);
    }

    // read pcap file
    pcap=pcap_open_offline(argv[2],errbuf);
    if(pcap==NULL) {
        cout << "ERROR: cannot open the file\n" << endl;
        exit(1);
    }
    cout << argv[2] << " is opened" << endl;
    
    int cnt=1;
    while(packet=pcap_next(pcap,&header)) {
        if(packet==NULL) break;
        parsePacket(packet,header.ts,&header);
        cout <<"packet No. " << cnt++ << endl; 
    }
    csv.writeToFile("test.csv");
    return 0;
}