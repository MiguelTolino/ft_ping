# Stage 1: Builder
FROM ubuntu:22.04 AS builder

# Install build-essential and make to compile the project
RUN apt-get update && apt-get install -y \
    build-essential \
    make \
    gcc \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Compile the project
RUN make

# Stage 2: Runtime
FROM ubuntu:22.04

# Install runtime dependencies and useful network tools for testing/debugging
# - iproute2: for 'ip' command
# - dnsutils: for 'dig' and 'nslookup'
# - iputils-ping: to compare against the original ping
# - tcpdump: for network packet analysis
RUN apt-get update && apt-get install -y \
    iproute2 \
    dnsutils \
    iputils-ping \
    tcpdump \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the compiled binary from the builder stage
# The Makefile puts the binary in 'build/ft_ping'
COPY --from=builder /app/build/ft_ping /usr/local/bin/ft_ping

# Ping requires privileges for raw socket operations.
# You can either:
# 1. Run the container with --cap-add=NET_RAW
# 2. Set the setuid bit on the binary (not recommended for containers, better to use caps)
# 3. Run as root (default in Docker)

# Use the binary as the entrypoint
ENTRYPOINT ["ft_ping"]

# Default command if no arguments are provided (shows help)
CMD ["--help"]
