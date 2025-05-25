#include "../../includes/ft_ping.h"
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

void init_ping(t_ping *ping, char *host)
{
    struct addrinfo hints;
    struct addrinfo *result;
    int status;

    if (!host || !*host)
        error_exit("Invalid host: empty hostname");

    memset(ping, 0, sizeof(t_ping));
    ping->host = host;
    ping->ttl = 64;
    ping->count = -1; // Default to infinite
    ping->interval = 1;
    ping->timeout = 0;  // 0 means no timeout by default
    ping->min_time = -1;
    ping->max_time = 0;
    ping->total_time = 0;

    // Resolve hostname to IP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if ((status = getaddrinfo(host, NULL, &hints, &result)) != 0)
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Unknown host: %s (%s)", host,
                 gai_strerror(status));
        error_exit(error_msg);
    }

    // Convert IP address to string
    if (inet_ntop(AF_INET, &((struct sockaddr_in *)result->ai_addr)->sin_addr,
                  ping->ip, MAX_IP_LEN) == NULL)
    {
        freeaddrinfo(result);
        error_exit("Invalid IP address");
    }

    freeaddrinfo(result);

    printf("PING %s (%s): %d data bytes", 
           host, ping->ip, PACKET_SIZE - 8);
    if (ping->verbose)
        printf(", id 0x%04x = %d", getpid() & 0xFFFF, getpid() & 0xFFFF);
    printf("\n");
}

void send_packet(t_ping *ping)
{
    char packet[PACKET_SIZE];
    struct icmphdr *icmp;
    struct sockaddr_in dest_addr;
    int ret;

    // Prepare ICMP packet
    memset(packet, 0, PACKET_SIZE);
    icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = getpid() & 0xFFFF;
    icmp->un.echo.sequence = ping->packets_sent;
    icmp->checksum = 0;
    icmp->checksum = in_cksum((unsigned short *)icmp, PACKET_SIZE);

    // Configure destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ping->ip);

    // Send the packet (PACKET_SIZE - sizeof(struct iphdr) to account for IP header)
    ret = sendto(ping->sockfd, packet, PACKET_SIZE - sizeof(struct iphdr), 0,
                 (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret < 0)
    {
        if (errno == EINTR)
            return;
        error_exit("sendto failed");
    }
    if (ret != PACKET_SIZE - sizeof(struct iphdr))
        error_exit("sendto: partial packet sent");

    ping->packets_sent++;
}

void receive_packet(t_ping *ping)
{
    char packet[MAX_PACKET_SIZE];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    struct timeval start_time, end_time;
    struct iphdr *ip;
    struct icmphdr *icmp;
    ssize_t bytes_received;
    int retries = 0;
    int max_retries = (strcmp(ping->ip, "127.0.0.1") == 0) ? 5 : 1;  // Increased retries for localhost
    const size_t min_packet_size = sizeof(struct iphdr) + sizeof(struct icmphdr);

    gettimeofday(&start_time, NULL);

    while (retries < max_retries)
    {
        // Receive response
        bytes_received = recvfrom(ping->sockfd, packet, MAX_PACKET_SIZE, 0,
                                 (struct sockaddr *)&from, &fromlen);

        if (bytes_received < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                retries++;
                if (retries < max_retries)
                {
                    usleep(100000);  // Sleep 100ms between retries
                    continue;
                }
                return;
            }
            if (errno == EINTR)
                return;
            error_exit("recvfrom failed");
        }

        // Validate minimum packet size
        if ((size_t)bytes_received < min_packet_size)
        {
            retries++;
            continue;
        }

        gettimeofday(&end_time, NULL);

        // Process received packet
        ip = (struct iphdr *)packet;
        icmp = (struct icmphdr *)(packet + (ip->ihl << 2));

        // Validate ICMP packet
        if (icmp->type != ICMP_ECHOREPLY)
        {
            retries++;
            continue;
        }

        // Validate packet ID
        if (icmp->un.echo.id != (getpid() & 0xFFFF))
        {
            retries++;
            continue;
        }

        double time_diff = get_time_diff(&start_time, &end_time);

        // Update statistics
        if (ping->min_time == -1 || time_diff < ping->min_time)
            ping->min_time = time_diff;
        if (time_diff > ping->max_time)
            ping->max_time = time_diff;
        ping->total_time += time_diff;
        
        // Store time for standard deviation calculation
        if (ping->packets_received < MAX_PACKETS)
            ping->times[ping->packets_received] = time_diff;
        
        ping->packets_received++;
        
        // Print response line
        printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms",
               bytes_received, ping->ip, icmp->un.echo.sequence, ip->ttl, time_diff);
        
        if (ping->verbose)
            printf(" id=0x%04x", icmp->un.echo.id);
        
        printf("\n");
        return;
    }
} 