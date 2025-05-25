#include "../../includes/ft_ping.h"

void error_exit(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

void print_usage(void)
{
    const char *usage =
        "Usage: ft_ping [OPTIONS] HOST\n"
        "Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
        "Options:\n"
        "  -v    Verbose output\n"
        "  -c    Stop after sending count packets\n"
        "  -t    Set TTL\n"
        "  -V    Display version information\n"
        "  -?    Display this help message\n\n"
        "Arguments:\n"
        "  HOST   The host to ping\n";
    printf("%s", usage);
    exit(0);
}

void print_version(void)
{
    const char *version = "ft_ping version 1.0\n";
    const char *author = "Written by mmateo-t\n";
    
    printf("%s%s", version, author);
    exit(0);
}