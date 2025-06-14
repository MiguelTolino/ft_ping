#include "../../includes/ft_ping.h"

double get_time_diff(struct timeval *start, struct timeval *end)
{
    long long start_us = (long long)start->tv_sec * 1000000LL + start->tv_usec;
    long long end_us = (long long)end->tv_sec * 1000000LL + end->tv_usec;
    return (end_us - start_us) / 1000.0;
}