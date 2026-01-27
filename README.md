# 🛰️ ft_ping

A recreation of the Linux ping command in C. This project implements the core functionality of the ping utility, allowing users to test network connectivity by sending ICMP ECHO_REQUEST packets to network hosts.

## 🛠️ Installation

### 📋 Prerequisites

- 📦 Required packages (on Debian/Ubuntu):
```bash
sudo apt-get update
sudo apt-get install -y build-essential gcc make iproute2
```

- 🛠️ GCC compiler
- 🔧 Make
- 🔐 Root privileges (for raw socket operations)
- 🐧 Linux operating system (not supported on macOS)

### 🏗️ Building

1. Clone the repository:
```bash
git clone https://github.com/MiguelTolino/ft_ping.git
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

### 🐳 Docker

If you're on macOS or don't want to install dependencies locally, you can use Docker:

1. Build the image:
```bash
docker build -t ft_ping .
```

2. Run with necessary capabilities for raw sockets:
```bash
docker run --rm --cap-add=NET_RAW ft_ping google.com
```

Or using docker-compose:
```bash
docker-compose run --rm ft_ping google.com
```

## 💻 Usage

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
ft_ping --ttl 32 google.com

# Set timeout to 5 seconds
ft_ping -w 5 google.com

# Set interval between packets to 2 seconds
ft_ping -i 2 google.com

# Verbose output
ft_ping -v 8.8.8.8

# Display version
ft_ping -V
```

## 🧪 Testing

Run the test documentation:
```bash
make test
```


## 🔧 Make Commands

| Command | Description |
|---------|-------------|
| `make` | 🏗️ Build the project |
| `make test` | 🧪 Run the test documentation |
| `make install` | 📥 Install ft_ping system-wide |
| `make uninstall` | 📤 Remove ft_ping from the system |
| `make clean` | 🧹 Remove build artifacts |
| `make fclean` | 🧹 Remove all generated files |
| `make re` | 🔄 Rebuild the project |
| `make debug` | 🐛 Build with debug flags |
| `make help` | ❓ Display available make commands |

## ⚠️ Error Handling

The program handles various error conditions:
- ❌ Invalid hostnames
- 🌐 Network connectivity issues
- 🚫 Invalid command-line options
- 🔒 Permission issues (requires root for raw sockets)
- ⏱️ Timeout handling for packet reception

## 🤝 Contributing

1. 🍴 Fork the repository
2. 🌿 Create your feature branch (`git checkout -b feature/amazing-feature`)
3. 💾 Commit your changes (`git commit -m 'Add some amazing feature'`)
4. 📤 Push to the branch (`git push origin feature/amazing-feature`)
5. 🔄 Open a Pull Request

## 📝 License

This project is part of the 42 school curriculum.

## 👨‍💻 Author

- mmateo-t

## 🙏 Acknowledgments

- Original ping command implementation
- 42 school for the project requirements
