#include <netdb.h>

void error(const char *message);
void check_input(int argc, char *argv[], char **port_number, char **name_of_server, char **user);
void print_addrinfo(struct addrinfo *address_info);
void get_message_from_json(char *message, char *message_json, char *key);
void setup_connection(const char *hostname, const char *port, int *sockfd);
void send_hello_message(int sockfd, const char *user, char *buffer);
void receive_game_id(int sockfd, char *game_id);
void play_game(int sockfd, const char *game_id);
