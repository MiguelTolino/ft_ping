#include "../includes/ft_ping.h"
#include <ctype.h>
#include <getopt.h>

t_ping g_ping;

static struct option long_options[] = {
    {"verbose", no_argument, 0, 'v'},
    {"version", no_argument, 0, 'V'},
    {"count", required_argument, 0, 'c'},
    {"ttl", required_argument, 0, 't'},
    {"timeout", required_argument, 0, 'w'},
    {"interval", required_argument, 0, 'i'},
    {"help", no_argument, 0, '?'},
    {0, 0, 0, 0}
};

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
        // Wait for the last packet's response before exiting
        if (g_ping.packets_sent > g_ping.packets_received)
        {
            struct timeval start_time, current_time;
            gettimeofday(&start_time, NULL);
            
            // Wait up to 1 second for the last packet
            while (1)
            {
                receive_packet(&g_ping);
                
                // If we received the packet or 1 second has passed, break
                if (g_ping.packets_sent == g_ping.packets_received)
                    break;
                    
                gettimeofday(&current_time, NULL);
                double elapsed = get_time_diff(&start_time, &current_time);
                if (elapsed >= 1000.0) // 1000ms = 1 second
                    break;
            }
        }
        print_statistics(&g_ping);
        cleanup_ping(&g_ping);
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

static int validate_ttl(t_ping_args *args, int value)
{
    if (value <= 0 || value > 255)
    {
        printf("ft_ping: ttl %d out of range\n", value);
        return (1);
    }
    args->ttl = value;
    return (0);
}

static int validate_count(t_ping_args *args, int value)
{
    if (value <= 0)
    {
        printf("ft_ping: bad number of packets to transmit\n");
        return (1);
    }
    args->count = value;
    return (0);
}

static int validate_timeout(t_ping_args *args, int value)
{
    if (value <= 0)
    {
        printf("ft_ping: bad timeout value\n");
        return (1);
    }
    args->timeout = value;
    return (0);
}

static int validate_interval(t_ping_args *args, int value)
{
    if (value <= 0)
    {
        printf("ft_ping: bad interval value\n");
        return (1);
    }
    args->interval = value;
    return (0);
}

int validate_arguments(int argc, char **argv, t_ping_args *args)
{
    int opt;
    int option_index = 0;
    
    // Initialize args with default values
    memset(args, 0, sizeof(t_ping_args));
    args->ttl = 64;
    args->count = -1;
    args->interval = 1;
    args->timeout = 0;
    args->verbose = 0;
    
    // Reset getopt for multiple calls
    optind = 1;
    
    while ((opt = getopt_long(argc, argv, "vVc:t:w:i:?", long_options, &option_index)) != -1)
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
                if (validate_count(args, atoi(optarg)))
                    return (1);
                break;
            case 't':
                if (validate_ttl(args, atoi(optarg)))
                    return (1);
                break;
            case 'w':
                if (validate_timeout(args, atoi(optarg)))
                    return (1);
                break;
            case 'i':
                if (validate_interval(args, atoi(optarg)))
                    return (1);
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
        args->host = argv[optind];
    
    if (!args->host)
    {
        printf("ft_ping: missing host operand\n");
        printf("Try 'ft_ping --help' for more information.\n");
        return (1);
    }
    
    return (0);
}

void run_ping_loop(t_ping *ping)
{
    struct timeval before, after;
    while (1)
    {
        // Check count limit
        if (ping->count > 0 && ping->packets_sent >= ping->count)
        {
            // Wait for the last packet's response before exiting
            if (ping->packets_sent == ping->count)
            {
                receive_packet(ping);
            }
            print_statistics(ping);
            cleanup_ping(ping);
            exit(0);
        }

        gettimeofday(&before, NULL);

        // Send and receive packet
        send_packet(ping);
        receive_packet(ping);

        gettimeofday(&after, NULL);
        double elapsed = get_time_diff(&before, &after); // in ms
        double interval_ms = (ping->interval > 0 ? ping->interval : 1) * 1000.0;

        // Handle timeout if specified
        if (ping->timeout > 0)
        {
            struct timeval current_time;
            gettimeofday(&current_time, NULL);
            double total_elapsed = get_time_diff(&before, &current_time);
            double timeout_ms = ping->timeout * 1000.0;

            if (total_elapsed >= timeout_ms)
            {
                print_statistics(ping);
                cleanup_ping(ping);
                exit(0);
            }

            // Calculate sleep time
            double remaining = timeout_ms - total_elapsed;
            if (remaining < interval_ms)
            {
                usleep(remaining * 1000);
                print_statistics(ping);
                cleanup_ping(ping);
                exit(0);
            }
        }

        // Sleep for the remaining interval, if any
        if (elapsed < interval_ms)
            usleep((interval_ms - elapsed) * 1000);
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

    // Cleanup before exit
    cleanup_ping(&g_ping);
    return (0);
} 