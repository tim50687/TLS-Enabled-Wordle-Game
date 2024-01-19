#include <sys/types.h>  // data types used in system call
#include <sys/socket.h> // structures needed for sockets
#include <netdb.h>      // for getaddrinfo()
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "cJSON.h"
#include "client.h"

#define SERVER_HOSTNAME "proj1.3700.network"

int main(int argc, char *argv[])
{
    // initialize port number, IP, and user
    char *port_number = NULL;
    char *name_of_server = NULL;
    char *user = NULL;

    // socket file descriptor
    int sockfd;

    // buffer to store data
    char *buffer;
    buffer = (char *)malloc(262144); // Allocate memory for the buffer

    // First, check user's command line input
    check_input(argc, argv, &port_number, &name_of_server, &user);

    // Get the address information
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints)); // set all bytes to 0
    // Provide preference
    hints.ai_family = AF_UNSPEC;     // allow both IPv4, IPv6
    hints.ai_socktype = SOCK_STREAM; // use TCP
    hints.ai_flags = AI_PASSIVE;     // usually used for server applications, if NULL is provided in 'node' paramter, getaddrinfo will automatically fill in IP

    if ((getaddrinfo(SERVER_HOSTNAME, port_number, &hints, &server_info)) < 0)
    {
        error("getaddrinfo error");
    }
    else
    {
        // print out some useful information to verify getaddrinfo
        print_addrinfo(server_info);
    }

    // Get the socket file descriptor
    if ((sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) == -1)
    {
        error("Client: socket");
    }

    // Connect to port
    if (connect(sockfd, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        close(sockfd);
        error("client: connect");
    }

    freeaddrinfo(server_info); // all done with server info after connection

    // Send Hello message
    send_hello_message(sockfd, user, buffer);

    // char hello_msg[1024];
    // int hello_len = 0;
    // sprintf(hello_msg, "{\"type\": \"hello\", \"northeastern_username\": \"%s\"}\n", user);
    // hello_len = strlen(hello_msg);

    // if (send(sockfd, hello_msg, hello_len, 0) == -1)
    // {
    //     error("Client said hello:");
    // }

    // // Receive Message from hello
    // int num_bytes;
    // if ((num_bytes = recv(sockfd, buffer, 4096, 0)) == -1)
    // {
    //     error("Client said hello, received from server: ");
    // }
    // buffer[num_bytes] = '\0';
    // printf("Client received %s", buffer);

    // Store the game ID
    char game_id[1024];
    get_message_from_json(game_id, buffer, "id");
    printf("game_id: %s\n", game_id);

    // Start guessing
    // Guess from given word list
    FILE *file = fopen("word_list.txt", "r");
    if (file == NULL)
    {
        error("Error opening file");
    }
    // Get the word
    char word[16];
    int num_bytes;
    while (fgets(word, sizeof(word), file) != NULL)
    {
        int guess_len = 0;
        char guess[1024];
        word[strlen(word) - 1] = '\0';
        sprintf(guess, "{\"type\": \"guess\", \"id\": \"%s\", \"word\": \"%s\"}\n", game_id, word);
        guess_len = strlen(guess);

        if (send(sockfd, guess, guess_len, 0) == -1)
        {
            error("Client guessed:");
        }

        // Receive Message after guessing
        memset(buffer, 0, 262144);
        if ((num_bytes = recv(sockfd, buffer, 4096, 0)) == -1)
        {
            error("Client guessed, received from server: ");
        }
        buffer[num_bytes] = '\0';
        // printf("Client received the result from guess %s\n", buffer);
        // Get the result
        char result[1024];
        get_message_from_json(result, buffer, "type");
        printf("result: %s\n", result);
    }

    fclose(file); // Close the file when done
    free(buffer);
    return 0;
}