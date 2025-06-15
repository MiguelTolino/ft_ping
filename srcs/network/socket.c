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
        tv.tv_sec = 0;  // Short timeout for remote hosts to avoid interference
        tv.tv_usec = 100000;  // 100ms timeout
    }
    
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        error_exit("setsockopt SO_RCVTIMEO failed");

    // Keep socket in blocking mode for accurate timing
    // Non-blocking mode can interfere with RTT measurement
} 