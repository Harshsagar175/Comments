# Content Distribution Network (CDN) Implementation

A distributed file storage and sharing system inspired by the ydj0604/Content-Distribution-Network repository, built with modern C++ and REST APIs.

## Architecture Overview

This CDN implementation consists of five main components:

### 1. **Origin Server** (Port 8081)
- Entry point for all client requests
- Routes requests to appropriate Meta Server
- Minimal computation, focuses on efficient routing

### 2. **Meta Server** (Port 8080) 
- Brain of the CDN system
- Maintains file metadata and CDN node information
- Implements intelligent CDN node selection algorithm:
  - Prefers CDN nodes that already have the file cached
  - Chooses geographically closest CDN node
  - Optimizes routing to avoid inefficient paths

### 3. **CDN Nodes** (Ports 8083, 8084, 8085)
- Local cache servers using LRU cache policy
- Handle file transfers between FSS and clients
- Implement prefetching strategy (fetch entire directories)
- Communicate directly with clients for file transfers

### 4. **File Storage System (FSS)** (Port 8082)
- Central storage datacenter
- Lightweight design with simple store/serve interface
- Easily replaceable and scalable

### 5. **Client Application**
- Command-line interface for upload/download operations
- Two-step process: check file metadata, then transfer files
- Supports file integrity verification

## Key Features

- **LRU Cache Policy** with automatic expiration
- **Geographic Distance Calculation** for optimal CDN selection
- **Prefetching Strategy** for improved cache hit rates
- **File Integrity Verification** using SHA-256 hashing
- **Heartbeat Monitoring** for CDN node health
- **RESTful API** design with JSON responses
- **Thread-safe Operations** with proper synchronization

## Project Structure

```
cdn-project/
├── CMakeLists.txt                 # Main build configuration
├── README.md                      # This file
├── config/
│   └── cdn.json                   # System configuration
├── include/
│   ├── common.h                   # Shared data structures
│   └── cdn_node.h                 # CDN node class definition
├── src/
│   ├── common/                    # Shared utilities
│   │   ├── CMakeLists.txt
│   │   ├── common.cpp
│   │   ├── utils.cpp
│   │   ├── logger.cpp
│   │   └── config.cpp
│   ├── origin_server/             # Origin server implementation
│   ├── meta_server/               # Meta server implementation
│   ├── cdn_node/                  # CDN node implementation
│   ├── fss/                       # File Storage System
│   └── client/                    # Client application
├── tests/
│   └── test_cdn.sh                # Test script
└── docs/                          # Documentation
```

## Dependencies

### Required Dependencies
- **C++17** or later compiler (GCC 8+, Clang 7+, MSVC 2017+)
- **CMake 3.16** or later
- **cpprestsdk** (Microsoft C++ REST SDK)
- **OpenSSL** (for SHA-256 hashing)
- **Threads** (usually part of standard library)

### Development Dependencies
- **curl** (for testing)
- **jq** (for JSON processing in tests)

## Installation Guide

### 1. Install Dependencies (Ubuntu/Debian)

```bash
# Update package manager
sudo apt update

# Install build tools
sudo apt install -y build-essential cmake git

# Install cpprestsdk and dependencies
sudo apt install -y libcpprest-dev libssl-dev

# Install testing tools
sudo apt install -y curl jq

# Install additional dependencies if needed
sudo apt install -y pkg-config libboost-all-dev
```

### 2. Install Dependencies (macOS)

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake openssl cpprestsdk curl jq
```

### 3. Install Dependencies (CentOS/RHEL/Fedora)

```bash
# For CentOS/RHEL 8+
sudo dnf install -y gcc-c++ cmake git openssl-devel curl jq

# Install cpprestsdk (may need EPEL repository)
sudo dnf install -y epel-release
sudo dnf install -y cpprest-devel

# For older versions, use yum instead of dnf
```

## Building the Project

### 1. Clone and Build

```bash
# Clone the repository
git clone <your-repo-url> cdn-project
cd cdn-project

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make -j$(nproc)

# Alternative: use cmake build
# cmake --build . --parallel
```

### 2. Install (Optional)

```bash
# Install to system directories
sudo make install

# Or install to custom prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make install
```

## Configuration

### 1. Edit Configuration File

Edit `config/cdn.json` to customize:
- Server IP addresses and ports
- CDN node locations (latitude/longitude)
- Cache sizes and TTL settings
- Storage paths

### 2. Create Required Directories

```bash
# Create data directories
mkdir -p data/{meta,fss_storage,cache_node_1,cache_node_2,cache_node_3}

# Create log directory
mkdir -p logs
```

## Running the System

### 1. Start All Components

```bash
# From the build directory
cd build

# Start Meta Server (Terminal 1)
./bin/meta_server ../config/cdn.json

# Start FSS (Terminal 2)
./bin/fss ../config/cdn.json

# Start Origin Server (Terminal 3)
./bin/origin_server ../config/cdn.json

# Start CDN Nodes (Terminals 4, 5, 6)
./bin/cdn_node ../config/cdn.json node_1
./bin/cdn_node ../config/cdn.json node_2
./bin/cdn_node ../config/cdn.json node_3
```

### 2. Automated Startup Script

```bash
# Create startup script
cat > start_cdn.sh << 'EOF'
#!/bin/bash
cd build
./bin/meta_server ../config/cdn.json &
sleep 2
./bin/fss ../config/cdn.json &
sleep 2
./bin/origin_server ../config/cdn.json &
sleep 2
./bin/cdn_node ../config/cdn.json node_1 &
./bin/cdn_node ../config/cdn.json node_2 &
./bin/cdn_node ../config/cdn.json node_3 &
echo "CDN system started!"
EOF

chmod +x start_cdn.sh
./start_cdn.sh
```

## Testing the System

### 1. Run Automated Tests

```bash
# Make sure all services are running first
./tests/test_cdn.sh
```

### 2. Manual Testing with curl

```bash
# Upload a file
curl -X POST -F "file=@testfile.txt" http://127.0.0.1:8081/upload

# Download a file (use file_id from upload response)
curl http://127.0.0.1:8081/download/{file_id} -o downloaded_file.txt

# List all files
curl http://127.0.0.1:8081/files

# Check CDN node status
curl http://127.0.0.1:8083/status
curl http://127.0.0.1:8084/status
curl http://127.0.0.1:8085/status
```

### 3. Test Client Application

```bash
# Upload files
./bin/client upload /path/to/file.txt

# Download files
./bin/client download {file_id} /path/to/destination

# List files
./bin/client list
```

## Performance Testing

### 1. Cache Performance Test

```bash
# First download (from FSS - slower)
time curl http://127.0.0.1:8081/download/{file_id} -o /dev/null

# Second download (from cache - faster)
time curl http://127.0.0.1:8081/download/{file_id} -o /dev/null
```

### 2. Geographic Distribution Test

Configure CDN nodes in different "locations" (different lat/long) and test which node gets selected for different client IPs.

### 3. Load Testing

```bash
# Install apache benchmark tool
sudo apt install apache2-utils

# Run load test
ab -n 1000 -c 10 http://127.0.0.1:8081/files
```

## Understanding the System

### Key Concepts to Learn

1. **Distributed System Design**
   - Service decomposition and communication
   - Stateless vs stateful services
   - Data consistency and eventual consistency

2. **Caching Strategies**
   - LRU (Least Recently Used) cache replacement
   - Cache invalidation and TTL (Time To Live)
   - Prefetching and cache warming

3. **Load Balancing & CDN Selection**
   - Geographic distance calculation (Haversine formula)
   - Latency-based routing decisions
   - Health monitoring and failover

4. **RESTful API Design**
   - HTTP methods and status codes
   - JSON request/response formatting
   - Error handling and logging

5. **File System Operations**
   - File integrity verification (SHA-256)
   - Atomic file operations
   - Directory traversal and cleanup

### Architecture Decisions

1. **Why separate Meta Server from CDN nodes?**
   - Centralized metadata management
   - Simplified consistency model
   - Easier to scale and maintain

2. **Why use LRU cache?**
   - Simple to implement and understand
   - Good performance for most access patterns
   - Built-in expiration mechanism

3. **Why geographic distance calculation?**
   - Reduces latency for end users
   - Simulates real CDN behavior
   - Easy to visualize and test

## Advanced Topics

### 1. Adding New CDN Nodes

1. Update `config/cdn.json` with new node information
2. Create cache directory for the new node
3. Start the new CDN node with unique node_id
4. Node will automatically register with Meta Server

### 2. Implementing Different Cache Policies

Replace LRU with:
- **LFU** (Least Frequently Used)
- **FIFO** (First In, First Out)
- **Random replacement**
- **Size-based eviction**

### 3. Adding Fault Tolerance

- Implement replica management in Meta Server
- Add CDN node failure detection and recovery
- Implement data replication across multiple CDN nodes

### 4. Performance Optimizations

- Add connection pooling for HTTP clients
- Implement async I/O for file operations
- Add compression for file transfers
- Use memory-mapped files for large file handling

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   ```bash
   # Missing cpprestsdk
   sudo apt install libcpprest-dev
   
   # Missing OpenSSL
   sudo apt install libssl-dev
   ```

2. **Runtime Errors**
   ```bash
   # Port already in use
   sudo netstat -tulpn | grep :8080
   sudo pkill -f cdn_
   
   # Permission denied for directories
   chmod -R 755 data/
   ```

3. **Network Issues**
   ```bash
   # Check if services are running
   curl http://127.0.0.1:8080/health
   
   # Check firewall
   sudo ufw status
   ```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes with tests
4. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## References

- [Original ydj0604 CDN Repository](https://github.com/ydj0604/Content-Distribution-Network)
- [Microsoft C++ REST SDK Documentation](https://github.com/Microsoft/cpprestsdk)
- [CDN Architecture Principles](https://developer.mozilla.org/en-US/docs/Glossary/CDN)
- [Cache Replacement Policies](https://en.wikipedia.org/wiki/Cache_replacement_policies)
