#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/time.h>
# include <signal.h>
# include <errno.h>
# include <math.h>

# define PACKET_SIZE 64
# define MAX_PACKET_SIZE 65507
# define MAX_IP_LEN 16
# define MAX_PACKETS 1000  // Maximum number of packets to store for stddev
# define MIN_INTERVAL_US 200  // Minimum interval in microseconds for flood ping protection

typedef struct s_ping_args {
    char        *host;
    int         ttl;
    int         count;
    int         interval;
    int         timeout;
    int         verbose;
} t_ping_args;

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
    double      times[MAX_PACKETS];  // Array to store round-trip times
} t_ping;

// Funciones principales
void    init_ping(t_ping *ping, t_ping_args *args);
void    cleanup_ping(t_ping *ping);
void    setup_socket(t_ping *ping);
void    send_packet(t_ping *ping);
void    receive_packet(t_ping *ping);
void    print_statistics(t_ping *ping);
void    run_ping_loop(t_ping *ping);

// Funciones de validación
int     validate_arguments(int argc, char **argv, t_ping_args *args);

// Funciones de manejo de señales
void    signal_handler(int signum);
void    setup_signal_handler(void);

// Funciones auxiliares
void    error_exit(const char *msg);
double  get_time_diff(struct timeval *start, struct timeval *end);
void    print_usage(void);
void    print_version(void);
unsigned short in_cksum(unsigned short *addr, int len);

#endif 