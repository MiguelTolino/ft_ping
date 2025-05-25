#include "../includes/ft_ping.h"
#include <unistd.h>

t_ping g_ping;

void print_statistics(t_ping *ping)
{
    printf("\n--- %s ping statistics ---\n", ping->host);
    printf("%d packets transmitted, %d received, ", 
        ping->packets_sent, ping->packets_received);
    if (ping->packets_sent > 0)
    {
        int loss = 100 - (ping->packets_received * 100 / ping->packets_sent);
        printf("%d%% packet loss\n", loss);
    }
    if (ping->packets_received > 0)
    {
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
            ping->min_time, ping->total_time / ping->packets_received, ping->max_time);
    }
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        print_statistics(&g_ping);
        exit(0);
    }
}

void setup_signal_handler(void)
{
    struct sigaction sa;
    
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) < 0)
        error_exit("Failed to set up signal handler");
}

int validate_arguments(int argc, char **argv)
{
    int i;
    char *host = NULL;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'v')
                g_ping.verbose = 1;
            else if (argv[i][1] == '?')
                print_usage();
            else
            {
                printf("ft_ping: invalid option -- '%c'\n", argv[i][1]);
                print_usage();
                return (1);
            }
        }
        else
            host = argv[i];
    }

    if (!host)
    {
        printf("ft_ping: missing host operand\n");
        print_usage();
        return (1);
    }

    g_ping.host = host;
    return (0);
}

void run_ping_loop(t_ping *ping)
{
    while (1)
    {
        send_packet(ping);
        receive_packet(ping);
        sleep(1); // Intervalo entre pings
    }
}

int main(int argc, char **argv)
{
    if (validate_arguments(argc, argv))
        return (1);

    // If help was requested, validate_arguments would have called print_usage()
    // and exited, so we only reach here if we have a valid host
    init_ping(&g_ping, g_ping.host);

    // Configurar manejador de señales
    setup_signal_handler();

    // Configurar socket
    setup_socket(&g_ping);

    // Bucle principal de ping
    run_ping_loop(&g_ping);

    return (0);
} 