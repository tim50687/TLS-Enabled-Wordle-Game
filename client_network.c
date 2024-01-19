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