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

int main(int argc, char *argv[])
{
    // initialize port number, IP, and user
    char *port_number = NULL;
    char *name_of_server = NULL;
    char *user = NULL;

    // socket file descriptor
    int sockfd = 0;

    // buffer to store data
    char *buffer;
    buffer = (char *)malloc(262144); // Allocate memory for the buffer

    // First, check user's command line input
    check_input(argc, argv, &port_number, &name_of_server, &user);

    // Get address information of server and get the socket file descriptor
    setup_connection(SERVER_HOSTNAME, port_number, &sockfd);

    // Send Hello message
    send_hello_message(sockfd, user, buffer); // now buffer will contain id

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