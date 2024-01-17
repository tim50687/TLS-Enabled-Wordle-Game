#include <sys/types.h>  // data types used in system call
#include <sys/socket.h> // structures needed for sockets
#include <netdb.h>      // for getaddrinfo()
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NON_TLS_PORT "27993"
#define TLS_PORT "27994"
#define SERVER_HOSTNAME "proj1.3700.network"

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
    if (argc < 3 || argc > 5)
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

/*
 * This function prints information from the given addrinfo structure.
 */
void print_addrinfo(struct addrinfo *address_info)
{
    printf("Family: %d\n", address_info->ai_family);
    printf("Socket Type: %d\n", address_info->ai_socktype);
    printf("IP address: ");
    char ip_str[1024];
    if (address_info->ai_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)address_info->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, sizeof(ip_str));
    }
    else
    {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)address_info->ai_addr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ip_str, sizeof(ip_str));
    }
    printf("%s\n", ip_str);
    printf("Port: %d\n", ntohs(((struct sockaddr_in *)address_info->ai_addr)->sin_port));
}

int main(int argc, char *argv[])
{
    // initialize port number, IP, and user
    char *port_number = NULL;
    char *name_of_server = NULL;
    char *user = NULL;

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

    freeaddrinfo(server_info);
    return 0;
}