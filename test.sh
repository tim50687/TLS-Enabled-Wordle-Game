#!/bin/bash

## Checking User input
echo "------------------------------------------------"
# Test case 1: Valid input with non-TLS port
./client -p 27993 server.example.com username
echo "------------------------------------------------"

# Test case 2: Valid input with TLS port
./client -s server.example.com username
echo "------------------------------------------------"

# Test case 3: Valid input with custom port
./client -p 12345 server.example.com username
echo "------------------------------------------------"

# Test case 4: Missing port argument
./client server.example.com username
echo "------------------------------------------------"

# Test case 5: Extra arguments
./client -p 27993 -s server.example.com username extra_argument
echo "------------------------------------------------"


