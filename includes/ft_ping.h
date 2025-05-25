#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/time.h>
# include <signal.h>
# include <errno.h>

# define PACKET_SIZE 64
# define MAX_PACKET_SIZE 65507
# define MAX_IP_LEN 16

// Definición de la estructura IP header
struct iphdr {
    unsigned int ihl : 4;
    unsigned int version : 4;
    unsigned char tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char ttl;
    unsigned char protocol;
    unsigned short check;
    unsigned int saddr;
    unsigned int daddr;
};

// Definición de la estructura ICMP header
struct icmphdr {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    union {
        struct {
            unsigned short id;
            unsigned short sequence;
        } echo;
        unsigned int gateway;
        struct {
            unsigned short mtu;
        } frag;
    } un;
};

typedef struct s_ping {
    char        *host;
    char        ip[MAX_IP_LEN];
    int         sockfd;
    int         ttl;
    int         count;
    int         interval;
    int         timeout;
    int         verbose;
    int         quiet;
    int         packets_sent;
    int         packets_received;
    double      min_time;
    double      max_time;
    double      total_time;
} t_ping;

// Funciones principales
void    init_ping(t_ping *ping, char *host);
void    setup_socket(t_ping *ping);
void    send_packet(t_ping *ping);
void    receive_packet(t_ping *ping);
void    print_statistics(t_ping *ping);
void    run_ping_loop(t_ping *ping);

// Funciones de validación
int     validate_arguments(int argc, char **argv);

// Funciones de manejo de señales
void    signal_handler(int signum);
void    setup_signal_handler(void);

// Funciones auxiliares
void    error_exit(const char *msg);
double  get_time_diff(struct timeval *start, struct timeval *end);
void    print_usage(void);
unsigned short in_cksum(unsigned short *addr, int len);

#endif 