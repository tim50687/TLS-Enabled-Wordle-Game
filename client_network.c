/**
 * Network Communication Functions for 3700.network Project - Client Application with TLS Support
 *
 * This file provides the implementation for network-related functions used in the client application
 * of the 3700.network project, including support for TLS (Transport Layer Security). Key functionalities
 * encompass establishing network connections with and without TLS encryption, managing data exchange
 * with the server, and implementing the initial communication protocol, including the "hello" message exchange.
 *
 * The functions utilize standard C libraries for socket programming along with OpenSSL for TLS encryption,
 * handling both sending and receiving data over network sockets in a secure manner. These functions are
 * essential in ensuring that the client application communicates effectively with the server, adhering to
 * the specified protocol, and maintaining data security through encrypted connections.
 *
 * Proper resource management and adherence to the server-defined protocol are crucial for successful
 * communication. This includes careful handling of socket descriptors and SSL contexts, particularly
 * in TLS-enabled exchanges. The implementation aims to provide a robust framework for secure and
 * reliable network communication within the project's client-server architecture.
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
 * @param ssl The SSL structure for TLS communication (can be NULL for non-TLS).
 * @param use_tls A flag indicating whether to use TLS (1 for TLS, 0 for non-TLS).
 *
 * Note: The buffer should be pre-allocated before calling this function. The function
 * will fill this buffer with the server's response. It's the caller's responsibility
 * to manage (allocate and free) this buffer.
 */
void send_hello_message(int sockfd, const char *user, char *buffer, SSL *ssl, int use_tls)
{
    char hello_msg[1024];
    int hello_len = 0;
    sprintf(hello_msg, "{\"type\": \"hello\", \"northeastern_username\": \"%s\"}\n", user);
    hello_len = strlen(hello_msg);
    int result = 0;
    int num_bytes = 0;

    if (use_tls) // TLS handshake
    {
        if (SSL_write(ssl, hello_msg, hello_len) < 0)
        {
            error("Client said hello:");
        }
        // Receive Message from hello
        if ((result = SSL_read(ssl, buffer, 1024)) == -1)
        {
            error("Client said hello, received from server: ");
        }
        else
        {
            num_bytes = strlen(buffer);
        }
    }
    else // Normal connection
    {
        if (send(sockfd, hello_msg, hello_len, 0) == -1)
        {
            error("Client said hello:");
        }

        // Receive Message from hello
        if ((num_bytes = recv(sockfd, buffer, 1024, 0)) == -1)
        {
            error("Client said hello, received from server: ");
        }
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
    //     print_addrinfo(server_info);
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

/**
 * Initializes the SSL library.
 *
 * This function loads all error strings and algorithms used by SSL. It is a prerequisite
 * before starting any SSL operations in the program. It should be called once during the
 * initialization phase of the application.
 */
void init_ssl()
{
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

/**
 * Creates and returns a new SSL context.
 *
 * This function sets up a new SSL context using the specified SSL method for a client.
 * It is essential for establishing TLS/SSL enabled connections.
 *
 * @return SSL_CTX* A pointer to the created SSL_CTX structure. On failure, the program
 *         will print an error message and exit.
 */
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

/**
 * Creates a new SSL object for a given SSL context.
 *
 * This function initializes a new SSL structure for a TLS/SSL connection.
 * The SSL structure is then bound to a socket file descriptor for communication.
 *
 * @param ctx The SSL context to be used for creating the SSL structure.
 * @param sockfd The socket file descriptor to be used for the SSL connection.
 * @return SSL* A pointer to the created SSL structure. On failure, the program
 *         will print an error message and exit.
 */
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

/**
 * Performs an SSL handshake to establish a secure connection.
 *
 * This function initiates the SSL/TLS handshake process with a server. The handshake
 * negotiates SSL parameters and, if successful, establishes a secure connection.
 *
 * @param ssl The SSL structure associated with the connection.
 * Note: The function will terminate the program with an error message if the handshake fails.
 */
void perform_ssl_handshake(SSL *ssl)
{
    if (SSL_connect(ssl) < 0)
    {
        perror("SSL handshake failed");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}
