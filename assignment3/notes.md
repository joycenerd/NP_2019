NP Assingment\#3: pcap File Reading and Statistics Program
==

###### tags: `Network Projgramming`

## Part1: pcap file parser

### Step1: Read in the pcap file

1. install **libpcap-dev**: `sudo apt-get install libpcap-dev`
1. read the pcap file into the program by `pcap_open_offline()`
2. read the packet one by one by `pcap_next()`

```cpp=
pcap=pcap_open_offline(argv[2],errbuf);

while(packet=pcap_next(pcap,&header)) {
    if(packet==NULL) break;
    parsePacket(packet,header.ts,&header);
    cout <<"packet No. " << cnt++ << endl; 
}
```

### Step2: Parse each packet

1. get the timestamp by `strftime()`
2. get source and destination MAC address
3. If it is a ip packet get the source and destination ip address by `inet_ntop()`
4. parse tcp, udp, icmp and igmp packet
5. Don't do anything with non-IP packet
6. write to csv file by `csv.newRow()` and `csv.writeToFile()`

```cpp=
// get the timestamp
strftime(buff,20,"%Y-%m-%d %H:%M:%S", localtime(&ts.tv_sec));

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

// ip header
inet_ntop(AF_INET,&(ip->ip_src),sourceIp,INET_ADDRSTRLEN);

// tcp header
tcpHeader=(tcphdr *)(packet+sizeof(struct ip));
sourcePort = ntohs(tcpHeader->source);

// udp header
udpHeader=(struct udphdr *)(packet+sizeof(struct ip));
sourcePort=htons(udpHeader->uh_sport);
```

## Part2: IP counter

1. while parsing ip packet, passed into a sub function to count the ip pairs (sourceIp,destIp)
2. if the pair exist then its counter +1
3. if the pair doesn't exist then add it to the vector and counter set to 1
4. Write to csv file

```cpp=
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
```

## Part3: Testing

My test data is from [Tcpreplay](http://tcpreplay.appneta.com/wiki/captures.html)