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

// Funciones auxiliares
void    error_exit(const char *msg);
double  get_time_diff(struct timeval *start, struct timeval *end);
void    print_usage(void);
unsigned short in_cksum(unsigned short *addr, int len);

#endif 