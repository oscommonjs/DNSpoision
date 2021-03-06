#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pcap.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ether.h>

#define SIZE_ETHERNET 14
#define BUFSIZE 65535

/*
 * Generic per-packet information, as supplied by libpcap.
 *
 * The time stamp can and should be a "struct timeval", regardless of
 * whether your system supports 32-bit tv_sec in "struct timeval",
 * 64-bit tv_sec in "struct timeval", or both if it supports both 32-bit
 * and 64-bit applications.  The on-disk format of savefiles uses 32-bit
 * tv_sec (and tv_usec); this structure is irrelevant to that.  32-bit
 * and 64-bit versions of libpcap, even if they're on the same platform,
 * should supply the appropriate version of "struct timeval", even if
 * that's not what the underlying packet capture mechanism supplies.
 *
struct pcap_pkthdr {
    struct timeval ts;  // time stamp
    bpf_u_int32 caplen; // length of portion present 
    bpf_u_int32 len;    // length this packet (off wire) 
};
*/

/* Ethernet header */
    struct ethernet_pack {
        u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
        u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
        u_short ether_type; /* IP? ARP? RARP? etc */
    };

    /* IP header */
    struct ip_pack {
        u_char ip_vhl;      /* version << 4 | header length >> 2 */
        u_char ip_tos;      /* type of service */
        u_short ip_len;     /* total length */
        u_short ip_id;      /* identification */
        u_short ip_off;     /* fragment offset field */
    #define IP_RF 0x8000        /* reserved fragment flag */
    #define IP_DF 0x4000        /* dont fragment flag */
    #define IP_MF 0x2000        /* more fragments flag */
    #define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
        u_char ip_ttl;      /* time to live */
        u_char ip_p;        /* protocol */
        u_short ip_sum;     /* checksum */
        struct in_addr ip_src;
        struct in_addr ip_dst; /* source and dest address */
    };
    #define IP_HL(ip)       (((ip)->ip_vhl) & 0x0f)
    #define IP_V(ip)        (((ip)->ip_vhl) >> 4)

    /* TCP header */
    struct tcp_pack {
        u_short th_sport;   /* source port */
        u_short th_dport;   /* destination port */
        u_int32_t th_seq;       /* sequence number */
        u_int32_t th_ack;       /* acknowledgement number */

        u_char th_offx2;    /* data offset, rsvd */
    #define TH_OFF(th)  (((th)->th_offx2 & 0xf0) >> 4)
        u_char th_flags;
    #define TH_FIN 0x01
    #define TH_SYN 0x02
    #define TH_RST 0x04
    #define TH_PUSH 0x08
    #define TH_ACK 0x10
    #define TH_SYNACK 0x12
    #define TH_URG 0x20
    #define TH_ECE 0x40
    #define TH_CWR 0x80
    #define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;     /* window */
        u_short th_sum;     /* checksum */
        u_short th_urp;     /* urgent pointer */
    };

    /*DNS header*/
    struct DNS_HEADER{
	    unsigned short id;       // identification number
	    unsigned char rd :1;     // recursion desired
	    unsigned char tc :1;     // truncated message
	    unsigned char aa :1;     // authoritive answer
	    unsigned char opcode :4; // purpose of message
	    unsigned char qr :1;     // query/response flag
	    unsigned char rcode :4;  // response code
	    unsigned char cd :1;     // checking disabled
	    unsigned char ad :1;     // authenticated data
	    unsigned char z :1;      // its z! reserved
	    unsigned char ra :1;     // recursion available
	    unsigned short q_count;  // number of question entries
	    unsigned short ans_count; // number of answer entries
	    unsigned short auth_count; // number of authority entries
	    unsigned short add_count; // number of resource entries
    };

    //Constant sized fields of query structure
    struct QUESTION{
	unsigned short qtype;
        unsigned short qclass;
    };
	 
    //Constant sized fields of the resource record structure
    struct R_DATA{
	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
    };

    //Pointers to resource record contents
    struct RES_RECORD{
	unsigned char *name;
	struct R_DATA *resource;
	unsigned char *rdata;
    };

    //Structure of a Query
    typedef struct{
	unsigned char *name;
	struct QUESTION *ques;
    } QUERY;
#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server
 
