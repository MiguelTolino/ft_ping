#!/bin/bash

sudo apt update
sudo apt install build-essential # For gcc, make, etc.
sudo apt install manpages-dev   # C library man pages
sudo apt install openssh-server # For SSH access
sudo apt install net-tools      # For ifconfig, netstat (optional, iproute2 is newer)
sudo apt install iproute2       # For ip addr, ss
sudo apt install dnsutils       # For nslookup, dig (useful for testing)
sudo apt install tcpdump        # Crucial for debugging network packets
sudo apt install inetutils-ping # For ping command
