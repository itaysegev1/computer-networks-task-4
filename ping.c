//
// Created by itayseg on 1/5/25.
//

#include <fcntl.h> // Definitions for file control options
#include <errno.h> // Error number definitions. Used for error handling (EACCES, EPERM)
#include <sys/socket.h> // Definitions for socket operations (socket, sendto, recvfrom)
#include <netdb.h> //Definitions for network database operations (for the hostent structure: gethostbyname)
#include <netinet/in.h> // Internet address family (AF_INET, AF_INET6)
#include <netinet/ip.h> // Definitions for internet protocol operations (IP header)
#include <netinet/ip6.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/ip_icmp.h> // Definitions for internet control message protocol operations (ICMP header)
#include <arpa/inet.h> // Definitions for internet operations (inet_pton, inet_ntoa)
#include <stdlib.h> // Standard input/output definitions
#include <strings.h> // String manipulation functions (bzero)
#include <string.h> // String manipulation functions (strlen, memset, memcpy)
#include <unistd.h> // UNIX standard function definitions (getpid, close, sleep)
#include <stdio.h> // Standard input/output definitions
#include <sys/wait.h> //Definitions for waiting/for holding processes (wait)

#define PACKETSIZE 64

struct packet {
    struct icmphdr hdr;
    char msg[PACKETSIZE - sizeof(struct icmphdr)];
};

int pid = -1;
int loops=4;
char *adress="0.0.0.0";
int ttl=56;
int sleepTime=1;
int ip_v=4;
struct protoent *proto=NULL;
int sent_packets = 0;
int received_packets = 0;
double min_rtt = -1, max_rtt = 0, total_rtt = 0;

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *) buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void displayipv4(void *buf, int bytes) {
    printf("-----ipv4------\n");
    printf((const char*)bytes);
    int i;
    struct iphdr *ip = (struct iphdr *) buf;
    struct icmphdr *icmp = (struct icmphdr *) (buf + ip->ihl * 4);
    printf("-------------------\n");
    for (i = 0; i < bytes; i += 2) {
        if (!(i % 15)) printf("\n%04x: ", i);
        printf("%02x ", ((unsigned char *) buf)[i]);
    }
    printf("\n");
    char sourceIPADDReadable[32] = {'\n'};
    char destinationIPADDReadable[32] = {'\n'};
    inet_ntop(AF_INET, &ip->saddr, sourceIPADDReadable, sizeof(sourceIPADDReadable));
    inet_ntop(AF_INET, &ip->daddr, destinationIPADDReadable, sizeof(destinationIPADDReadable));
    printf("ipv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s \n"
           , ip->version, ip->ihl * 4, ntohs(ip->tot_len), ip->protocol, ip->ttl, sourceIPADDReadable);
    printf("dst=%s\n", destinationIPADDReadable);
    if (icmp->un.echo.id == pid) {
        printf("ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n", icmp->type, icmp->code,
               icmp->checksum, icmp->un.echo.id, icmp->un.echo.sequence);
    }
}

void displayipv6(void *buf, int bytes) {
    printf("-----ipv6------\n");
    printf((const char*)bytes);
    int i;
    struct ip6_hdr *ip = (struct ipv6hdr *) buf;
    struct icmphdr *icmp = (struct icmphdr *) (buf + sizeof(struct ip6_hdr));
    printf("-------------------\n");
    for (i = 0; i < bytes; i += 2) {
        if (!(i % 15)) printf("\n%04x: ", i);
        printf("%02x ", ((unsigned char *) buf)[i]);
    }
    printf("\n");
    char sourceIPADDReadable[INET6_ADDRSTRLEN] = {'\n'};
    char destinationIPADDReadable[INET6_ADDRSTRLEN] = {'\n'};
    inet_ntop(AF_INET6,&ip->ip6_src,sourceIPADDReadable,sizeof(sourceIPADDReadable));
    inet_ntop(AF_INET6,&ip->ip6_dst, destinationIPADDReadable, sizeof(destinationIPADDReadable));
    printf("ipv6: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s \n"
           , 15,15,15,15,"15");
    printf("dst=%s\n", destinationIPADDReadable);
    if (icmp->un.echo.id == pid) {
        printf("ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n", icmp->type, icmp->code,
               icmp->checksum, icmp->un.echo.id, icmp->un.echo.sequence);
    }
}

void display(void *buf, int bytes) {
    printf("-------------- display -----------\n");
    if (ip_v == 4) {
        displayipv4(buf, bytes);
    }
    else if (ip_v ==6) {
        displayipv6(buf, bytes);
    }
    else {
        printf("Unknown version\n");
    }
}
void listener(void)
{	int sd;
    struct sockaddr_in addr;
    unsigned char buf[1024];

    sd = socket(PF_INET, SOCK_RAW, proto->p_proto);
    if ( sd < 0 )
    {
        perror("socket");
        exit(0);
    }
    //for (;;)
    for ( int i = 0; i < loops; i++ )
    {
        int bytes, len=sizeof(addr);

        bzero(buf, sizeof(buf));
        bytes = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &len);
        if (bytes < 0)
            perror("recvfrom failed");
        if ( bytes > 0 )
            display(buf, bytes);
        else
            perror("recvfrom");
        sleep(1);
    }
    exit(0);
}

void ping(struct  sockaddr_in *addr) {
    if (!proto) {
        perror("ICMP protocol not found");
        exit(0);
    }
    int i,j,sd,cnt=1;
    struct packet pckt;
    struct sockaddr_in r_addr;

    sd=socket(PF_INET, SOCK_RAW, proto->p_proto);
    if (sd<0) {
        perror("socket");
        return;
    }
    if (setsockopt(sd,SOL_IP,IP_TTL,&ttl,sizeof(ttl)) != 0) {
        perror("set TTL option");
    }
    if (fcntl(sd,F_SETFL, O_NONBLOCK) != 0) {
        perror("Request unblocking I/O");
    }
    for (int i=0; i<loops; i++) {
        int len=sizeof(r_addr);
        printf("Msg #%d\n to %s",cnt,adress);
        if (recvfrom(sd,&pckt,sizeof(pckt),0, (struct sockaddr*)&r_addr, &len) >0) {
            printf("****GOT MESSAGE****");
        }
        bzero(&pckt,sizeof(pckt));
        pckt.hdr.type=ICMP_ECHO;
        pckt.hdr.un.echo.id=pid;
        for (int i=0; i<sizeof(pckt.msg); i++) {
            pckt.msg[i]=i+'0';
        }
        pckt.msg[i]=0;
        pckt.hdr.checksum=checksum(&pckt,sizeof(pckt));
        pckt.hdr.un.echo.sequence=cnt++;

        if (sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 ) {
            perror("sendto");
        }
        sleep(sleepTime);
    }
}

int main(int argc, char *argv[]) {
    struct hostent *hname;
    struct sockaddr_in addr;
    if (argc<5) {
        printf("Usage: %s <hostname> <port> <ttl>\n", argv[0]);
        exit(0);
    }
    for (int i=1; i<argc;i+=2) {
        printf(argv[i]);
        printf(" , ");
        printf(argv[i+1]);
        if (strcmp(argv[i],"-a") == 0) {
            adress=argv[i+1];
            hname=gethostbyname(argv[i+1]);
        }
        if (strcmp(argv[i],"-c") == 0) {
            loops=atoi(argv[i+1]);
        }
        if (strcmp(argv[i],"-f") == 0) {
            sleepTime=0;
        }
        if (strcmp(argv[i],"-t") == 0) {
            if (strcmp(argv[i+1],"4") == 0) {
                ip_v=4;
            }else {
                if (strcmp(argv[i+1],"6") == 0) {
                    ip_v=6;
                }
                else {
                    perror("unknown ip version");
                    return -1;
                }
            }
        }
    }
    if (argc>4) {
        pid=getpid();
        proto=getprotobyname("ICMP");
        bzero(&addr,sizeof(addr));
        addr.sin_family=hname->h_addrtype;
        addr.sin_port=0;
        memcpy((char *)&addr.sin_addr.s_addr,hname->h_addr_list[0],hname->h_length);
        if (fork()==0) {
            printf("listener");
            listener();
        }
        else {
            printf("ping");
            ping(&addr);
        }
        wait(0);
    }
    else {
        return 0;
    }

}



