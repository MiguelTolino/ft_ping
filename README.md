# ft_ping

A recreation of the Linux ping command in C. This project implements the core functionality of the ping utility, allowing users to test network connectivity by sending ICMP ECHO_REQUEST packets to network hosts.

## Features

- Send ICMP ECHO_REQUEST packets to network hosts
- Display round-trip time statistics
- Support for various command-line options:
  - `-c`: Stop after sending count packets
  - `-t`: Set the IP Time To Live
  - `-v`: Verbose output
  - `-V`: Display version information
  - `-?`: Display help message

## Installation

### Prerequisites

- GCC compiler
- Make
- Root privileges (for raw socket operations)

### Building

1. Clone the repository:
```bash
git clone https://github.com/yourusername/ft_ping.git
cd ft_ping
```

2. Build the project:
```bash
make
```

3. Install (optional):
```bash
sudo make install
```

## Usage

Basic usage:
```bash
ft_ping [OPTIONS] HOST
```

Examples:
```bash
# Basic ping
ft_ping google.com

# Send 5 packets
ft_ping -c 5 8.8.8.8

# Set TTL to 32
ft_ping -t 32 google.com

# Verbose output
ft_ping -v 8.8.8.8

# Display version
ft_ping -V
```

## Testing

Run the test suite:
```bash
make test
```

The tests compare ft_ping's behavior with the original ping command and generate detailed logs in the `logs` directory.

## Project Structure

```
ft_ping/
├── includes/
│   └── ft_ping.h
├── srcs/
│   ├── main.c
│   ├── network/
│   │   ├── packet.c
│   │   └── socket.c
│   └── utils/
│       ├── checksum.c
│       ├── error.c
│       └── time.c
├── tests/
│   └── test_ping.sh
├── Makefile
└── README.md
```

## Make Commands

- `make`: Build the project
- `make test`: Run the test suite
- `make install`: Install ft_ping system-wide
- `make uninstall`: Remove ft_ping from the system
- `make clean`: Remove build artifacts
- `make fclean`: Remove all generated files
- `make re`: Rebuild the project
- `make debug`: Build with debug flags
- `make help`: Display available make commands

## Error Handling

The program handles various error conditions:
- Invalid hostnames
- Network connectivity issues
- Invalid command-line options
- Permission issues (requires root for raw sockets)

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is part of the 42 school curriculum.

## Author

- mmateo-t

## Acknowledgments

- Original ping command implementation
- 42 school for the project requirements
