#include "../includes/ft_ping.h"

static t_ping g_ping;

static void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        printf("\n--- %s ping statistics ---\n", g_ping.host);
        printf("%d packets transmitted, %d received, ", 
            g_ping.packets_sent, g_ping.packets_received);
        if (g_ping.packets_sent > 0)
        {
            int loss = 100 - (g_ping.packets_received * 100 / g_ping.packets_sent);
            printf("%d%% packet loss\n", loss);
        }
        if (g_ping.packets_received > 0)
        {
            printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
                g_ping.min_time, g_ping.total_time / g_ping.packets_received, g_ping.max_time);
        }
        exit(0);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        print_usage();
        return (1);
    }

    // Inicializar estructura ping
    init_ping(&g_ping, argv[1]);

    // Configurar manejador de señales
    signal(SIGINT, signal_handler);

    // Configurar socket
    setup_socket(&g_ping);

    // Bucle principal de ping
    while (1)
    {
        send_packet(&g_ping);
        receive_packet(&g_ping);
        sleep(1); // Intervalo entre pings
    }

    return (0);
} 