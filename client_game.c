/**
 * Client Application for the 3700.network Project
 *
 * This source file implements the core functionalities of the client application,
 * including game logic for guessing, and management of game resources.
 * It utilizes cJSON for JSON parsing and a set data structure for maintaining game state.
 */

#include "client.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "set.h" // using set data structure

/**
 * Parses the last guess and its hints from the server response.
 * @param guesses_from_server The JSON string containing guesses from the server.
 * @param hint The array to store the hint (correctly guessed characters).
 * @param garbage_alphabets Set of alphabets that are not in the word.
 */
void retreive_last_hint(const char *guesses_from_server, char *hint, SimpleSet garbage_alphabets)
{
    cJSON *json = cJSON_Parse(guesses_from_server);
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

    // Access guesses in json object
    cJSON *guesses = cJSON_GetObjectItemCaseSensitive(json, "guesses");
    if (!cJSON_IsArray(guesses))
    {
        fprintf(stderr, "Error: guesses is not an array\n");
        cJSON_Delete(json);
        return;
    }

    // Traverse the array from the end
    cJSON *last_guess = NULL;
    if (cJSON_GetArraySize(guesses) > 0)
    {
        last_guess = cJSON_GetArrayItem(guesses, cJSON_GetArraySize(guesses) - 1);
    }

    // Retreive the latest guess and hint for future guess
    if (last_guess != NULL)
    {
        cJSON *word = cJSON_GetObjectItemCaseSensitive(last_guess, "word");
        cJSON *marks = cJSON_GetObjectItemCaseSensitive(last_guess, "marks");

        // Get the latest guess
        if (word->valuestring == NULL)
        {
            error("Guess word: ");
        }
        // Get the latest hint
        if (cJSON_IsArray(marks))
        {
            int num_marks = cJSON_GetArraySize(marks);
            for (int i = 0; i < num_marks; i++)
            {
                cJSON *mark = cJSON_GetArrayItem(marks, i);

                if (cJSON_IsNumber(mark) && mark->valueint == 2) // check if the hint is 2
                {
                    // hint structure will look like
                    // index 0,  1,  2,  3,  4
                    // value nul, a, e, nul, nul
                    hint[i] = word->valuestring[i];
                }
                else if (cJSON_IsNumber(mark) && mark->valueint == 0) // check if the hint is 0
                {
                    // add the alphabet not in used to set data structure
                    set_add(&garbage_alphabets, &(word->valuestring[i]));
                }
            }
        }
    }
    cJSON_Delete(json);
}

/**
 * Checks if the given word is a good guess based on the hint and garbage alphabets.
 * @param word The word to check.
 * @param hint Array of hints where correctly guessed characters are stored.
 * @param garbage_alphabets Set of alphabets not in the answer.
 * @return 1 if the word is a good guess, 0 otherwise.
 */
int is_good_word_to_guess(const char *word, const char *hint, SimpleSet *garbage_alphabets)
{
    for (int i = 0; word[i] != '\0'; i++)
    {
        if (hint[i] != '\0' && word[i] != hint[i])
        {
            // Discard word that does not match tthe hint
            return 0;
        }
        if (set_contains(garbage_alphabets, &word[i]) == SET_TRUE)
        {
            // Found garbage alphabet, return 0
            return 0;
        }
    }
    return 1;
}

/**
 * Cleans up game resources including allocated memory for buffer, hint, and garbage alphabets set.
 *
 * @param buffer A pointer to a dynamically allocated memory buffer, to be freed.
 * @param hint A pointer to a dynamically allocated hint array, to be freed.
 * @param garbage_alphabets A pointer to a SimpleSet struct, which holds the set of garbage alphabets.
 *
 */
void cleanup_game_resources(char **buffer, char **hint, SimpleSet *garbage_alphabets)
{
    if (buffer != NULL && *buffer != NULL)
    {
        free(*buffer);
        *buffer = NULL;
    }
    if (hint != NULL && *hint != NULL)
    {
        free(*hint);
        *hint = NULL;
    }
    if (garbage_alphabets != NULL)
    {
        set_destroy(garbage_alphabets);
    }
}

/**
 * Plays the guessing game by sending guesses to the server and processing responses.
 * @param sockfd The socket file descriptor for non-TLS communication.
 * @param ssl The SSL structure for TLS communication (can be NULL for non-TLS).
 * @param game_id The game ID for the current session.
 * @param use_tls A flag indicating whether to use TLS (1 for TLS, 0 for non-TLS).
 */
void play_game(int sockfd, const char *game_id, SSL *ssl, int use_tls)
{
    // Guess from given word list
    FILE *file = fopen("word_list.txt", "r");
    if (file == NULL)
    {
        error("Error opening file");
    }
    // Get the word
    char word[16];
    int num_bytes;

    // Message received
    char *buffer;
    buffer = (char *)malloc(262144); // Allocate memory for the buffer

    // Get the hint that store the correct guess of alphabet, aka correct alphabet with correct position
    char *hint;
    hint = (char *)malloc(32);
    memset(hint, '\0', 32); // Initialize the hint array to null characters

    // Collect the garbage alphabet not in the answer.
    SimpleSet garbage_alphabets;
    set_init(&garbage_alphabets);

    // Store the message from server if the guess was correct
    char correct_message[256];

    while (fgets(word, sizeof(word), file) != NULL)
    {
        // Verify next word
        if (!is_good_word_to_guess(word, hint, &garbage_alphabets))
        {
            continue;
        }
        int guess_len = 0;
        char guess[1024];
        word[strlen(word) - 1] = '\0';
        sprintf(guess, "{\"type\": \"guess\", \"id\": \"%s\", \"word\": \"%s\"}\n", game_id, word);
        guess_len = strlen(guess);

        if (use_tls) // TLS handshake
        {
            if (SSL_write(ssl, guess, guess_len) < 0)
            {

                error("Client guess:");
            }
            // Receive message after guessing
            memset(buffer, 0, 262144);
            int result = SSL_read(ssl, buffer, 262144);
            if (result < 0)
            {
                error("Cllient guessed, received from server ");
            }
            else
            {
                num_bytes = strlen(buffer);
            }
        }
        else // Normal connection
        {

            if (send(sockfd, guess, guess_len, 0) == -1)
            {
                error("Client guessed:");
            }

            // Receive Message after guessing
            memset(buffer, 0, 262144);
            if ((num_bytes = recv(sockfd, buffer, 262144, 0)) == -1)
            {
                error("Client guessed, received from server: ");
            }
        }

        buffer[num_bytes] = '\0';

        // If guess the correct answer
        get_message_from_json(correct_message, buffer, "type");
        if (strcmp(correct_message, "bye") == 0)
        {
            char ans[64];
            get_message_from_json(ans, buffer, "flag");
            printf("%s\n", ans);
            break;
        }

        // Get the hint by server
        retreive_last_hint(buffer, hint, garbage_alphabets);

        // Get the result
        char result[1024];
        get_message_from_json(result, buffer, "type");
    }
    fclose(file); // Close the file when done

    cleanup_game_resources(&buffer, &hint, &garbage_alphabets);
}