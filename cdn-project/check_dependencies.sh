#!/bin/bash

echo "=== CDN Dependencies Check ==="

# Check for compiler
if command -v g++ &> /dev/null; then
    echo "✓ g++ compiler found: $(g++ --version | head -n1)"
else
    echo "✗ g++ compiler not found"
fi

# Check for cmake
if command -v cmake &> /dev/null; then
    echo "✓ cmake found: $(cmake --version | head -n1)"
else
    echo "✗ cmake not found"
fi

# Check for pkg-config
if command -v pkg-config &> /dev/null; then
    echo "✓ pkg-config found"
else
    echo "✗ pkg-config not found"
fi

# Check for curl
if command -v curl &> /dev/null; then
    echo "✓ curl found: $(curl --version | head -n1)"
else
    echo "✗ curl not found"
fi

# Check for jq
if command -v jq &> /dev/null; then
    echo "✓ jq found: $(jq --version)"
else
    echo "✗ jq not found"
fi

echo ""
echo "=== Package Checks ==="

# Check for cpprestsdk
if pkg-config --exists cpprest; then
    echo "✓ cpprestsdk found: $(pkg-config --modversion cpprest)"
else
    echo "? cpprestsdk not found via pkg-config (may still be available)"
fi

# Check for openssl
if pkg-config --exists openssl; then
    echo "✓ OpenSSL found: $(pkg-config --modversion openssl)"
else
    echo "? OpenSSL not found via pkg-config (may still be available)"
fi

echo ""
echo "=== Installation Suggestions ==="
echo "Ubuntu/Debian:"
echo "  sudo apt install -y build-essential cmake libcpprest-dev libssl-dev curl jq"
echo ""
echo "macOS:"
echo "  brew install cmake openssl cpprestsdk curl jq"
echo ""
echo "CentOS/RHEL:"
echo "  sudo dnf install -y gcc-c++ cmake cpprest-devel openssl-devel curl jq"

