#include "../includes/ft_ping.h"
#include <ctype.h>

t_ping g_ping;

void print_statistics(t_ping *ping)
{
    printf("\n--- %s ping statistics ---\n", ping->host);
    printf("%d packets transmitted, %d packets received, ", 
        ping->packets_sent, ping->packets_received);
    if (ping->packets_sent > 0)
    {
        int loss = 100 - (ping->packets_received * 100 / ping->packets_sent);
        printf("%d%% packet loss\n", loss);
    }
    if (ping->packets_received > 0)
    {
        double avg = ping->total_time / ping->packets_received;
        double stddev = 0.0;
        
        // Calculate standard deviation
        for (int i = 0; i < ping->packets_received; i++)
        {
            double diff = ping->times[i] - avg;
            stddev += diff * diff;
        }
        stddev = sqrt(stddev / ping->packets_received);
        
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
            ping->min_time, avg, ping->max_time, stddev);
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

int validate_arguments(int argc, char **argv, t_ping_args *args)
{
    int opt;
    char *host = NULL;
    
    // Reset getopt for multiple calls (if needed)
    optind = 1;
    
    // Initialize args with default values
    memset(args, 0, sizeof(t_ping_args));
    args->ttl = 64;
    args->count = -1;
    args->interval = 1;
    args->timeout = 0;
    args->verbose = 0;
    
    while ((opt = getopt(argc, argv, "vVc:t:w:i:?")) != -1)
    {
        switch (opt)
        {
            case 'v':
                args->verbose = 1;
                break;
            case 'V':
                print_version();
                break;
            case 'c':
                args->count = atoi(optarg);
                if (args->count <= 0)
                {
                    printf("ft_ping: bad number of packets to transmit\n");
                    return (1);
                }
                break;
            case 't':
                args->ttl = atoi(optarg);
                if (args->ttl <= 0 || args->ttl > 255)
                {
                    printf("ft_ping: ttl %d out of range\n", args->ttl);
                    return (1);
                }
                break;
            case 'w':
                args->timeout = atoi(optarg);
                if (args->timeout <= 0)
                {
                    printf("ft_ping: bad timeout value\n");
                    return (1);
                }
                break;
            case 'i':
                args->interval = atoi(optarg);
                if (args->interval <= 0)
                {
                    printf("ft_ping: bad interval value\n");
                    return (1);
                }
                break;
            case '?':
                print_usage();
                return (1);
            default:
                print_usage();
                return (1);
        }
    }
    
    // Get the host argument (non-option argument)
    if (optind < argc)
        host = argv[optind];
    
    if (!host)
    {
        printf("ft_ping: missing host operand\n");
        return (1);
    }
    
    args->host = host;
    return (0);
}

void run_ping_loop(t_ping *ping)
{
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    while (1)
    {
        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        
        // Check if we've exceeded the timeout
        if (ping->timeout > 0)
        {
            double elapsed = get_time_diff(&start_time, &current_time);
            if (elapsed >= ping->timeout)
            {
                print_statistics(ping);
                exit(0);
            }
        }

        // Check if we've reached the count limit before sending
        if (ping->count > 0 && ping->packets_sent >= ping->count)
        {
            print_statistics(ping);
            exit(0);
        }

        send_packet(ping);
        receive_packet(ping);
        
        // Use custom interval if specified, otherwise default to 1 second
        sleep(ping->interval > 0 ? ping->interval : 1);
    }
}

int main(int argc, char **argv)
{
    t_ping_args args;

    if (validate_arguments(argc, argv, &args))
        return (1);

    // If help was requested, validate_arguments would have called print_usage()
    // and exited, so we only reach here if we have a valid host
    init_ping(&g_ping, &args);

    // Configurar manejador de señales
    setup_signal_handler();

    // Configurar socket
    setup_socket(&g_ping);

    // Bucle principal de ping
    run_ping_loop(&g_ping);

    return (0);
} 