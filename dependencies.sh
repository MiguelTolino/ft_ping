#!/bin/bash

# Update package lists without interaction
sudo apt-get update -y

# Install dependencies without interaction
sudo apt-get install -y build-essential # For gcc, make, etc.
sudo apt-get install -y manpages-dev   # C library man pages
sudo apt-get install -y openssh-server # For SSH access
sudo apt-get install -y net-tools      # For ifconfig, netstat (optional, iproute2 is newer)
sudo apt-get install -y iproute2       # For ip addr, ss
sudo apt-get install -y dnsutils       # For nslookup, dig (useful for testing)
sudo apt-get install -y tcpdump        # Crucial for debugging network packets
sudo apt-get install -y inetutils-ping # For ping command

# Clean up
sudo apt-get clean
sudo apt-get autoremove -y
