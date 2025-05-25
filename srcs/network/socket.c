#include "../../includes/ft_ping.h"

void setup_socket(t_ping *ping)
{
    // Crear socket raw para ICMP
    if ((ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        error_exit("socket creation failed");

    // Establecer TTL
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ping->ttl,
                   sizeof(ping->ttl)) < 0)
        error_exit("setsockopt failed");

    // Establecer timeout de recepción
    struct timeval tv;
    tv.tv_sec = 1;  // 1 second timeout for receiving individual packets
    tv.tv_usec = 0;
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        error_exit("setsockopt failed");
} 