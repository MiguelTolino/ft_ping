#include "../../includes/ft_ping.h"

void error_exit(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

void print_usage(void)
{
    const char *usage =
        "ping: missing host operand\n"
        "Try 'ping --help' or 'ping --usage' for more information.\n";
    printf("%s", usage);
    exit(1);
} 