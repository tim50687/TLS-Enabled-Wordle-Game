/**
 * client.h
 *
 * Header file for the client application of the 3700.network project.
 * This file contains declarations for the network communication and game logic functions,
 * as well as necessary includes and constant definitions used throughout the client application.
 *
 * Functions in this file are responsible for setting up network connections,
 * handling communication with the server, parsing server responses, and managing
 * the game logic for the client.
 *
 * Constants defined in this file are used for server hostname and port numbers.
 * These constants provide a centralized location for these configurations,
 * making it easier to modify them if necessary.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <netdb.h> // For struct addrinfo

// Constant definitions
#define SERVER_HOSTNAME "proj1.3700.network"
#define NON_TLS_PORT "27993"
#define TLS_PORT "27994"

void error(const char *message);
void check_input(int argc, char *argv[], char **port_number, char **name_of_server, char **user);
void print_addrinfo(struct addrinfo *address_info);
void get_message_from_json(char *message, char *message_json, char *key);
void setup_connection(const char *hostname, const char *port, int *sockfd);
void send_hello_message(int sockfd, const char *user, char *buffer);
void play_game(int sockfd, const char *game_id);

#endif // CLIENT_H