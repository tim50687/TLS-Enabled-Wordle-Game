/**
 * Implementation of network communication functions for the client application of the 3700.network project.
 *
 * This file contains the implementation of functions responsible for setting up network connections,
 * handling communication with the server, and managing the initial "hello" message protocol.
 * It utilizes standard networking libraries in C for socket programming and handles the transmission
 * and reception of data over the network.
 *
 * Functions:
 * - setup_connection: Establishes a connection to the server.
 * - send_hello_message: Sends a greeting message to the server and receives a response.
 *
 * Usage of these functions requires proper handling of resources, such as socket file descriptors,
 * and ensuring that network communication adheres to the protocol defined by the server.
 */

#include "client.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * Sends a "hello" message to the server and receives the response.
 * This function constructs a JSON-formatted hello message, sends it to the server,
 * and then waits to receive a response from the server. The response is stored in the provided buffer.
 *
 * @param sockfd The socket file descriptor for communicating with the server.
 * @param user The username to be included in the hello message.
 * @param buffer A buffer to store the response received from the server.
 *
 * Note: The buffer should be pre-allocated before calling this function. The function
 * will fill this buffer with the server's response. It's the caller's responsibility
 * to manage (allocate and free) this buffer.
 */
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
}

/**
 * Sets up a connection to the server using the specified hostname and port number.
 * This function resolves the server's address and port, creates a socket, and establishes
 * a connection to the server. It updates the provided socket file descriptor.
 *
 * @param hostname The server's hostname or IP address to connect to.
 * @param port_number The port number on the server to connect to.
 * @param sockfd A pointer to an int that will store the socket file descriptor.
 *
 * Note: This function is responsible for resolving the server address and establishing
 * a connection. It's the caller's responsibility to close the socket file descriptor
 * when the connection is no longer needed.
 */
void setup_connection(const char *hostname, const char *port_number, int *sockfd)
{
    // Get the address information
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints)); // Set all bytes to 0
    // Provide preference
    hints.ai_family = AF_UNSPEC;     // Allow both IPv4, IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP
    hints.ai_flags = AI_PASSIVE;     // Usually used for server applications, if NULL is provided in 'node' paramter, getaddrinfo will automatically fill in IP

    if ((getaddrinfo(hostname, port_number, &hints, &server_info)) < 0)
    {
        error("getaddrinfo error");
    }
    // else
    // {
    //     // Print out some useful information to verify getaddrinfo
    //     // print_addrinfo(server_info);
    // }

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

    freeaddrinfo(server_info); // All done with server info after connection
}

void init_ssl()
{
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

SSL_CTX *create_ssl_context()
{
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;
}

SSL *create_ssl_object(SSL_CTX *ctx, int sockfd)
{
    SSL *ssl = SSL_new(ctx);
    if (!ssl)
    {
        perror("Unable to create SSL object");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // set the socket for SSL
    SSL_set_fd(ssl, sockfd);

    return ssl;
}

// Function to perform SSL handshake
void perform_ssl_handshake(SSL *ssl)
{
    if (SSL_connect(ssl) != 1)
    {
        perror("SSL handshake failed");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}
