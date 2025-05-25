#!/bin/bash

# Update package lists without interaction
apt-get update -y

# Install dependencies without interaction
apt-get install -y build-essential # For gcc, make, etc.
apt-get install -y manpages-dev   # C library man pages
apt-get install -y openssh-server # For SSH access
apt-get install -y net-tools      # For ifconfig, netstat (optional, iproute2 is newer)
apt-get install -y iproute2       # For ip addr, ss
apt-get install -y dnsutils       # For nslookup, dig (useful for testing)
apt-get install -y tcpdump        # Crucial for debugging network packets
apt-get install -y inetutils-ping # For ping command

# Clean up
apt-get clean
apt-get autoremove -y
