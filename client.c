/**
 * Main entry point for the client application of the 3700.network project.
 *
 * This file contains the main function that orchestrates the flow of the client application,
 * handling command-line arguments, establishing a connection with the server,
 * sending the initial greeting message, and starting the game process.
 * It relies on various helper functions defined in other parts of the project.
 */

#include <sys/types.h>  // Data types used in system call
#include <sys/socket.h> // Structures needed for sockets
#include <netdb.h>      // getaddrinfo()
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "cJSON.h"
#include "client.h"

/**
 * Main function of the client application.
 */
int main(int argc, char *argv[])
{
    // Initialize port number, IP, and user
    char *port_number = NULL;
    char *name_of_server = NULL;
    char *user = NULL;

    // Socket file descriptor
    int sockfd = 0;

    // For TSL handshake
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    int use_tls = 0;

    // Buffer to store message from server, in order to get game_id
    char *buffer;
    buffer = (char *)malloc(1024); // Allocate memory for the buffer

    // First, check user's command line input
    check_input(argc, argv, &port_number, &name_of_server, &user);

    // Get address information of server and get the socket file descriptor
    setup_connection(SERVER_HOSTNAME, port_number, &sockfd);

    // Check if port number is 27994 -> TSL handshake
    if (strcmp(port_number, "27994") == 0)
    {
        init_ssl();
        // Create and configure SSL context
        ctx = create_ssl_context();
        // Create and configure SSL object
        ssl = create_ssl_object(ctx, sockfd);
        // Perform SSL handshake
        perform_ssl_handshake(ssl);

        use_tls = 1;

        printf("TSL handshake set up successfully\n");
    }

    // Send Hello message
    send_hello_message(sockfd, user, buffer, ssl, use_tls); // Now buffer will contain id

    // Store the game ID
    char game_id[1024];
    get_message_from_json(game_id, buffer, "id");

    // Play game
    play_game(sockfd, game_id, ssl, use_tls);

    // Shutdown and cleanup
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);

    // Cleanup OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    // free the allocated buffer
    free(buffer);

    return 0;
}