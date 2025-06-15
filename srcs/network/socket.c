#include "../../includes/ft_ping.h"
#include <fcntl.h>

void setup_socket(t_ping *ping)
{

    // Create raw socket for ICMP
    if ((ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        error_exit("socket creation failed");


    // Set TTL
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ping->ttl,
                   sizeof(ping->ttl)) < 0)
        error_exit("setsockopt IP_TTL failed");

    // Optimize socket buffer sizes for better performance
    int buffer_size = 65536;
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0)
        error_exit("setsockopt SO_RCVBUF failed");
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) < 0)
        error_exit("setsockopt SO_SNDBUF failed");

    // Set balanced receive timeout for performance and reliability  
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;  // 1s timeout for reliable packet reception
    
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        error_exit("setsockopt SO_RCVTIMEO failed");

    // Enable socket timestamp for precise timing
    int timestamp_flag = 1;
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_TIMESTAMP, &timestamp_flag, sizeof(timestamp_flag)) < 0)
        error_exit("setsockopt SO_TIMESTAMP failed");

    // Keep socket in blocking mode for accurate timing
    // Non-blocking mode can interfere with RTT measurement
} 