#include <sys/types.h>  // data types used in system call
#include <sys/socket.h> // structures needed for sockets
#include <netdb.h>      // for getaddrinfo()
#include <stdlib.h>
#include <stdio.h>

#define NON_TLS_PORT "27993"
#define TLS_PORT "27994"

/*
 * This function is used to display an error message provided in the 'message'
 * parameter using the perror function. It then terminates the program
 * with an exit status of 1 (indicating an error condition).
 */
void error(const char *message)
{
    perror(message);
    exit(1);
}

/*
 * Check command-line arguments and set port number, server name, and user
 */
void check_input(int argc, char *argv[], char **port_number, char **name_of_server, char **user)
{
    if (argc < 3 || argc > 6)
    {
        fprintf(stderr, "Usage: ./client <-p port> <-s> <hostname> <Northeastern-username>\n");
        exit(1);
    }

    // Only handle 4 possible inputs, Hopefully it's Ok
    if (argc == 3)
    {
        *port_number = NON_TLS_PORT;
    }
    else if (argc == 4)
    {
        *port_number = TLS_PORT;
    }
    else if (argc == 5)
    {
        *port_number = argv[2];
    }
    else
    {
        fprintf(stderr, "Usage: ./client <-p port> <-s> <hostname> <Northeastern-username>\n");
        exit(1);
    }

    // Get the name_of_server and user
    *name_of_server = argv[argc - 2];
    *user = argv[argc - 1];

    // Print out the result
    printf("Port number: %s\n", *port_number);
    printf("Name of the server: %s\n", *name_of_server);
    printf("User name: %s\n", *user);
}

int main(int argc, char *argv[])
{
    // initialize port number, IP, and user
    char *port_number = NULL;
    char *name_of_server = NULL;
    char *user = NULL;

    // First, check user's command line input
    check_input(argc, argv, &port_number, &name_of_server, &user);
    return 0;
}