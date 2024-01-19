#include "client.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void send_hello_message(int sockfd, const char *user, char *buffer)
{
    char hello_msg[1024];
    int hello_len = 0;
    sprintf(hello_msg, "{\"type\": \"hello\", \"northeastern_username\": \"%s\"}\n", user);
    hello_len = strlen(hello_msg);

    if (send(sockfd, hello_msg, hello_len, 0) == -1)
    {
        error("Client said hello:");
    }

    // Receive Message from hello
    int num_bytes;
    if ((num_bytes = recv(sockfd, buffer, 4096, 0)) == -1)
    {
        error("Client said hello, received from server: ");
    }
    buffer[num_bytes] = '\0';
    printf("Client received %s", buffer);
}

void setup_connection(const char *hostname, const char *port_number, int *sockfd)
{
    // Get the address information
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints)); // set all bytes to 0
    // Provide preference
    hints.ai_family = AF_UNSPEC;     // allow both IPv4, IPv6
    hints.ai_socktype = SOCK_STREAM; // use TCP
    hints.ai_flags = AI_PASSIVE;     // usually used for server applications, if NULL is provided in 'node' paramter, getaddrinfo will automatically fill in IP

    if ((getaddrinfo(hostname, port_number, &hints, &server_info)) < 0)
    {
        error("getaddrinfo error");
    }
    else
    {
        // print out some useful information to verify getaddrinfo
        print_addrinfo(server_info);
    }

    // Get the socket file descriptor
    if ((*sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) == -1)
    {
        error("Client: socket");
    }

    // Connect to port
    if (connect(*sockfd, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        close(*sockfd);
        error("client: connect");
    }

    freeaddrinfo(server_info); // all done with server info after connection
}