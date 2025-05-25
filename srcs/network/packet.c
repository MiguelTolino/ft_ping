#include "../../includes/ft_ping.h"

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
    ping->count = -1; // Infinito por defecto
    ping->interval = 1;
    ping->timeout = 1;
    ping->min_time = -1;

    // Resolver el nombre de host a dirección IP
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

    // Convertir la dirección IP a string
    if (inet_ntop(AF_INET, &((struct sockaddr_in *)result->ai_addr)->sin_addr,
                  ping->ip, MAX_IP_LEN) == NULL)
    {
        freeaddrinfo(result);
        error_exit("Invalid IP address");
    }

    freeaddrinfo(result);

    printf("PING %s (%s): %d data bytes, id 0x%04x = %d\n", 
           host, ping->ip, PACKET_SIZE - 8, getpid() & 0xFFFF, getpid() & 0xFFFF);
}

void send_packet(t_ping *ping)
{
    char packet[PACKET_SIZE];
    struct icmphdr *icmp;
    struct sockaddr_in dest_addr;

    // Preparar el paquete ICMP
    memset(packet, 0, PACKET_SIZE);
    icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = getpid() & 0xFFFF;
    icmp->un.echo.sequence = ping->packets_sent;
    icmp->checksum = 0;
    icmp->checksum = in_cksum((unsigned short *)icmp, PACKET_SIZE);

    // Configurar dirección de destino
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ping->ip);

    // Enviar el paquete
    if (sendto(ping->sockfd, packet, PACKET_SIZE, 0,
               (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
        error_exit("sendto failed");

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
    int bytes_received;

    gettimeofday(&start_time, NULL);

    // Recibir respuesta
    bytes_received = recvfrom(ping->sockfd, packet, MAX_PACKET_SIZE, 0,
                             (struct sockaddr *)&from, &fromlen);

    if (bytes_received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        return;
    }

    gettimeofday(&end_time, NULL);

    // Procesar el paquete recibido
    ip = (struct iphdr *)packet;
    icmp = (struct icmphdr *)(packet + (ip->ihl << 2));

    if (icmp->type == ICMP_ECHOREPLY && icmp->un.echo.id == (getpid() & 0xFFFF))
    {
        double time_diff = get_time_diff(&start_time, &end_time);

        if (ping->min_time == -1 || time_diff < ping->min_time)
            ping->min_time = time_diff;
        if (time_diff > ping->max_time)
            ping->max_time = time_diff;
        ping->total_time += time_diff;
        
        // Store the time for standard deviation calculation
        if (ping->packets_received < MAX_PACKETS)
            ping->times[ping->packets_received] = time_diff;
        
        ping->packets_received++;
        
        // Print response line (normal output)
        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms",
               bytes_received, ping->ip, icmp->un.echo.sequence, ip->ttl, time_diff);
        
        // Add verbose information if requested
        if (ping->verbose)
        {
            printf(" id=0x%04x", icmp->un.echo.id);
        }
        
        printf("\n");
    }
} 