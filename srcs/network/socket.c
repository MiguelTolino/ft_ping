#include "../../includes/ft_ping.h"
#include <fcntl.h>

void setup_socket(t_ping *ping)
{
    int opt = 1;

    // Create raw socket for ICMP
    if ((ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        error_exit("socket creation failed");

    // Set socket options
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0)
        error_exit("setsockopt SO_RCVBUF failed");

    // Set TTL
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ping->ttl,
                   sizeof(ping->ttl)) < 0)
        error_exit("setsockopt IP_TTL failed");

    // Set receive timeout
    struct timeval tv;
    // Use shorter timeout for localhost
    if (strcmp(ping->ip, "127.0.0.1") == 0)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 100000;  // 100ms for localhost
    }
    else
    {
        tv.tv_sec = 1;  // 1 second for remote hosts
        tv.tv_usec = 0;
    }
    
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        error_exit("setsockopt SO_RCVTIMEO failed");

    // Set non-blocking mode
    int flags = fcntl(ping->sockfd, F_GETFL, 0);
    if (flags < 0)
        error_exit("fcntl F_GETFL failed");
    if (fcntl(ping->sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
        error_exit("fcntl F_SETFL failed");
} 