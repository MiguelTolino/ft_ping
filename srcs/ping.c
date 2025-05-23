#include "../includes/ft_ping.h"

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

// Función para calcular el checksum ICMP
unsigned short in_cksum(unsigned short *addr, int len) {
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1) {
    *(unsigned char *)(&answer) = *(unsigned char *)w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

void init_ping(t_ping *ping, char *host) {
  struct addrinfo hints;
  struct addrinfo *result;
  int status;

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
    error_exit("getaddrinfo failed");

  // Convertir la dirección IP a string
  inet_ntop(AF_INET, &((struct sockaddr_in *)result->ai_addr)->sin_addr,
            ping->ip, MAX_IP_LEN);
  freeaddrinfo(result);

  printf("PING %s (%s) %d bytes of data.\n", host, ping->ip, PACKET_SIZE);
}

void setup_socket(t_ping *ping) {
  // Crear socket raw para ICMP
  if ((ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    error_exit("socket creation failed");

  // Establecer TTL
  if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ping->ttl,
                 sizeof(ping->ttl)) < 0)
    error_exit("setsockopt failed");

  // Establecer timeout de recepción
  struct timeval tv;
  tv.tv_sec = ping->timeout;
  tv.tv_usec = 0;
  if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    error_exit("setsockopt failed");
}

void send_packet(t_ping *ping) {
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

void receive_packet(t_ping *ping) {
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

  if (bytes_received < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      printf("Request timeout for icmp_seq %d\n", ping->packets_sent - 1);
    return;
  }

  gettimeofday(&end_time, NULL);

  // Procesar el paquete recibido
  ip = (struct iphdr *)packet;
  icmp = (struct icmphdr *)(packet + (ip->ihl << 2));

  if (icmp->type == ICMP_ECHOREPLY && icmp->un.echo.id == (getpid() & 0xFFFF)) {
    double time_diff = get_time_diff(&start_time, &end_time);

    if (ping->min_time == -1 || time_diff < ping->min_time)
      ping->min_time = time_diff;
    if (time_diff > ping->max_time)
      ping->max_time = time_diff;
    ping->total_time += time_diff;
    ping->packets_received++;

    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
           bytes_received - (ip->ihl << 2), inet_ntoa(from.sin_addr),
           icmp->un.echo.sequence, ip->ttl, time_diff);
  }
}

double get_time_diff(struct timeval *start, struct timeval *end) {
  return ((end->tv_sec - start->tv_sec) * 1000.0 +
          (end->tv_usec - start->tv_usec) / 1000.0);
}

void error_exit(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void print_usage(void) {
  const char *usage =
      "Usage: ft_ping [options] host\n"
      "\nOptions:\n"
      "  -c count     Stop after sending count packets\n"
      "  -i interval  Wait interval seconds between sending each packet\n"
      "  -t ttl       Set the IP Time To Live\n"
      "  -v           Verbose output\n"
      "  -q           Quiet output\n"
      "  -h           Display this help and exit\n"
      "\nExample:\n"
      "  ft_ping -c 4 -i 2 google.com\n";
  printf("%s", usage);
  exit(1);
}