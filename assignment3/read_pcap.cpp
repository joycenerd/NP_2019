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
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <vector>
using namespace std;


// structure of ip pairs
struct ipPairs {
    string sourceIp;
    string destIp;
    int counter;
};


CSVWriter csv;  // for all packet information
CSVWriter csv2; // for counter of ip
vector<struct ipPairs>ips;  // counter of ip


// get MAC address
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
    return mac;
}

// count IP pairs
void ipCounter(string source, string dest) {
    for(int i=0;i<ips.size();i++) {
        if(source==ips[i].sourceIp && dest==ips[i].destIp){
            ips[i].counter++;
            return;
        }
    }
    struct ipPairs tmp;
    tmp.sourceIp=source;
    tmp.destIp=dest;
    tmp.counter=1;
    ips.push_back(tmp);
    return;
} 

// packet parser
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
    struct udphdr *udpHeader;
    struct icmphdr *icmpHeader;

    // get timestamp
    strftime(buff,20,"%Y-%m-%d %H:%M:%S", localtime(&ts.tv_sec));
    csv.newRow() << buff;

    // get source and destination MAC address
    ethernetHeader=(struct ether_header *)packet;
    packet+=sizeof(struct ether_header);
    sourceMAC=printAddr(ethernetHeader->ether_shost);
    destMAC=printAddr(ethernetHeader->ether_dhost);

    // if the packet is IP header
    if(ntohs(ethernetHeader->ether_type)==ETHERTYPE_IP) {

        // get source and destination IP address
        ip=(struct ip *)packet; 
        inet_ntop(AF_INET,&(ip->ip_src),sourceIp,INET_ADDRSTRLEN);
        inet_ntop(AF_INET,&(ip->ip_dst),destIp,INET_ADDRSTRLEN);
        ipCounter(sourceIp,destIp); // count ip pairs

        // parse tcp header
        if(ip->ip_p==IPPROTO_TCP) {
        tcpHeader=(tcphdr *)(packet+sizeof(struct ip));
        sourcePort = ntohs(tcpHeader->source);
        destPort = ntohs(tcpHeader->dest);
        //cout << sourcePort << " " << destPort << endl;
        csv<< sourceMAC << sourceIp << sourcePort << destMAC << destIp << destPort << "tcp";
        }

        // parse udp header
        else if(ip->ip_p==IPPROTO_UDP) {
            udpHeader=(struct udphdr *)(packet+sizeof(struct ip));
            sourcePort=htons(udpHeader->uh_sport);
            destPort=htons(udpHeader->uh_dport);
            csv<< sourceMAC << sourceIp << sourcePort << destMAC << destIp << destPort << "udp";
        }

        // parse icmp header
        else if(ip->ip_p==IPPROTO_ICMP) {
            csv<< sourceMAC << sourceIp << " " << destMAC << destIp << " " << "icmp";
        }

        // parse igmp header
        else if(ip->ip_p==IPPROTO_IGMP){
            csv<< sourceMAC << sourceIp << " " << destMAC << destIp;
        }
    }

    // not IP packet
    else {
        csv<< sourceMAC << " " << " " << destMAC;

    }

    return;
}



int main(int argc, char *argv[]) {

    pcap_t *pcap;
    char errbuf[PCAP_ERRBUF_SIZE];
    const unsigned char *packet;
    struct pcap_pkthdr header;

    // column name
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

    // read packet one by one
    while(packet=pcap_next(pcap,&header)) {
        if(packet==NULL) break;
        parsePacket(packet,header.ts,&header);
        cout <<"packet No. " << cnt++ << endl; 
    }
    csv.writeToFile("packet_info.csv");

    // ip counter
    csv2.newRow() << "Source IP" << "Target IP" << "Counter";
    for(int i=0;i<ips.size();i++) {
        csv2.newRow() << ips[i].sourceIp << ips[i].destIp << ips[i].counter;
    }
    csv2.writeToFile("counter.csv");

    return 0;
}