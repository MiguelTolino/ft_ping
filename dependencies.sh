#!/bin/bash

sudo apt-get update
sudo apt-get install build-essential # For gcc, make, etc.
sudo apt-get install manpages-dev   # C library man pages
sudo apt-get install openssh-server # For SSH access
sudo apt-get install net-tools      # For ifconfig, netstat (optional, iproute2 is newer)
sudo apt-get install iproute2       # For ip addr, ss
sudo apt-get install dnsutils       # For nslookup, dig (useful for testing)
sudo apt-get install tcpdump        # Crucial for debugging network packets
