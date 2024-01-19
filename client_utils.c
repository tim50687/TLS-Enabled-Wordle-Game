/**
 * Source file for the client application of the 3700.network project.
 *
 * This file contains the implementation of functions for
 * command-line argument processing, error handling, and JSON message parsing.
 */

#include "client.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cJSON.h"

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

/**
 * Extracts a specific message from a JSON string based on the provided key.
 *
 * This function parses a JSON string and retrieves the value associated with
 * the specified key. The value is then copied to the 'message' parameter.
 *
 * @param message A buffer where the extracted message will be stored.
 * @param message_json The JSON string to parse.
 * @param key The key for which the value is to be extracted from the JSON string.
 *
 * Note:
 * - The 'message' buffer should be large enough to hold the extracted value.
 * - If the key is not found or the corresponding value is not a string, no action is taken.
 * - In case of a JSON parsing error, an error message is printed to the standard output.
 */
void get_message_from_json(char *message, char *message_json, char *key)
{
    cJSON *json = cJSON_Parse(message_json); // Use library deal with JSON
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return;
    }
    // Access the JSON data
    cJSON *target_message = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(target_message) && (target_message->valuestring != NULL))
    {
        // Copy to target message to message
        strcpy(message, target_message->valuestring);
    }
    // Delete the JSON object
    cJSON_Delete(json);
}