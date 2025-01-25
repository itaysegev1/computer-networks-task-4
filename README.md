# 🌐 Network Tools Implementation

This project implements two fundamental network diagnostic tools in C: `ping` and `traceroute`. These tools are essential for network troubleshooting and understanding network paths and latency.

## 📦 Project Components

### 🔍 1. Ping Implementation (`ping.c`)
A custom implementation of the ping utility that supports both IPv4 and IPv6. This tool sends ICMP ECHO requests to a specified destination and measures the round-trip time (RTT).

Features:
- ✅ Support for both IPv4 and IPv6 protocols
- 🔢 Configurable number of ping requests
- ⚡ Flood mode option for stress testing
- 📊 Comprehensive statistics including packet loss and RTT metrics
- ⏰ Timeout handling (10-second timeout)

### 🗺️ 2. Traceroute Implementation (`traceroute.c`)
A traceroute utility that maps the network path between your computer and a destination host. It shows each hop along the route and measures transit delays.

Features:
- 🛣️ Supports up to 30 hops
- 🔄 Sends 3 ICMP packets per hop for reliability
- ⏱️ Displays RTT for each hop
- 🏢 Shows IP addresses of intermediate routers
- ⚠️ Timeout handling for unresponsive hops

## 🛠️ Prerequisites

- 🐧 Linux-based operating system (Ubuntu 22.04 LTS recommended)
- 🔨 GCC compiler
- 👑 Root privileges (for creating raw sockets)

## 🏗️ Build Instructions

The project uses a Makefile system for easy compilation. Here are the available make commands:

```bash
# Build everything (both ping and traceroute)
make

# Build individual programs
make ping
make tracerout

# Clean build files
make clean
```

### Makefile Details
The provided Makefile includes:
- Compiler optimization and warning flags
- Automatic dependency handling
- Clean build system
- Math library linking

## 🚀 Usage Instructions

### 📡 Ping Usage

Basic syntax:
```bash
sudo ./ping -a <address> -t <type>
```

Required flags:
- 📍 `-a <address>`: Target IP address to ping
- 🔄 `-t <type>`: IP version (4 for IPv4, 6 for IPv6)

Optional flags:
- 🔢 `-c <count>`: Number of ping requests to send
- ⚡ `-f`: Enable flood mode (rapid ping)

Examples:
```bash
# IPv4 ping with 5 requests
sudo ./ping -a 8.8.8.8 -t 4 -c 5

# IPv6 ping in flood mode
sudo ./ping -a 2001:4860:4860::8888 -t 6 -f
```

### 🗺️ Traceroute Usage

Basic syntax:
```bash
sudo ./traceroute -a <address>
```

Required flag:
- 📍 `-a <address>`: Target IP address to trace

Example:
```bash
sudo ./traceroute -a 8.8.8.8
```

## 📝 Output Examples

### Ping Output
```
PING 8.8.8.8 with 64 bytes of data:
64 bytes from 8.8.8.8: icmp_seq=1 ttl=117 time=5.980ms
64 bytes from 8.8.8.8: icmp_seq=2 ttl=117 time=6.830ms
...
--- 8.8.8.8 ping statistics ---
4 packets transmitted, 4 received, time 3028.23ms
rtt min/avg/max = 5.980/7.05/8.450 ms
```

### Traceroute Output
```
traceroute to 8.8.8.8, 30 hops max
 1  172.18.192.1 0.565ms 0.455ms 0.274ms
 2  10.100.102.1 1.568ms 1.301ms 1.192ms
 ...
 8  8.8.8.8 6.904ms 5.958ms 7.652ms
```

## 🔧 Technical Notes

- 🛡️ Both programs use raw sockets and require root privileges
- 📦 ICMP packets are constructed manually including checksum calculations
- 🌐 IPv6 support includes proper ICMPv6 filtering
- ⚡ Programs handle timeouts and errors gracefully
- 📊 Statistics are calculated and displayed upon completion/interruption

## ⚠️ Error Handling

- 🔍 Both programs include comprehensive error checking
- ❌ Invalid inputs are caught and appropriate error messages are displayed
- 🕒 Network timeouts and unreachable hosts are handled appropriately
- 🛑 Signal handling for graceful termination

## ❗ Important Notes

- 👑 Must be run with sudo due to raw socket requirements
- 🐧 Designed for and tested on Ubuntu 22.04 LTS
- 🚫 Some routers may be configured to not respond to ICMP packets
- 🌐 For IPv6 functionality, ensure your system has IPv6 connectivity
