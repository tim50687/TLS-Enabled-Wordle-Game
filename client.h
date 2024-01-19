#include <netdb.h>

void error(const char *message);
void check_input(int argc, char *argv[], char **port_number, char **name_of_server, char **user);
void print_addrinfo(struct addrinfo *address_info);
void get_message_from_json(char *message, char *message_json, char *key);
void setup_connection(const char *hostname, const char *port, int *sockfd);
void send_hello_message(int sockfd, const char *user, char *buffer);
void play_game(int sockfd, const char *game_id);

#define SERVER_HOSTNAME "proj1.3700.network"
#define NON_TLS_PORT "27993"
#define TLS_PORT "27994"