# Socket Basics - Wordle Client Implementation

### Description
This project involves implementing a client program for a variant of the Wordle game. The client communicates with a server, making guesses for a secret word and receiving feedback on the accuracy of these guesses. The game's goal is to correctly guess the secret word, upon which the server returns a unique secret flag. The client supports both non-encrypted and `TLS` encrypted socket communication, retrieving different secret flags for each.

### Environment and Dependencies
- **Language**: The program is written in C.
- **External Libraries**:
  - **cJSON**: Used for JSON parsing and encoding. [cJSON](https://github.com/DaveGamble/cJSON/blob/master)
  - **Set**: Used for managing sets of characters. [set/src](https://github.com/barrust/set/tree/master/src)
- **OpenSSL**: Required for TLS encrypted communication. Installed via `Homebrew`.
- **Compiler**: GCC or any standard C compiler.
- **Build System**: Makefile for compiling the project.

### Compilation
To compile the project, a Makefile is included. Run the following command in the terminal:
```
make
```

### Execution
To run the client program, use the following command syntax:
```
./client <-p port> <-s> <hostname> <Northeastern-username>
```
- `-p port`: Optional, specifies the TCP port of the server.
- `-s`: Optional, indicates the use of a TLS encrypted socket.
- `hostname`: Required, the server's name or IP address.
- `Northeastern-username`: Required, the user's Northeastern username.

### Wordle Game Strategy
The client employs a linear search strategy to guess the secret word from `word_list.txt`:
1. **Skip Letters (0 Marks)**: A `set` data structure stores incorrect letters (marked as 0). Words containing these letters will simply be skipped.
2. **Correct Alphabet (2 Marks)**: A character pointer stores correctly positioned letters. Words not matching these positions are skipped.


### Testing
The testing process for the client program consisted of two main approaches:

1. **Automated Script Testing**:
   - Developed test scripts were used to validate `user input` handling on the terminal. These tests ensured that the client program correctly parses and reacts to different command-line arguments and inputs.

2. **Manual Testing Against the Server**:
   - The client was rigorously tested by connecting to the server at `proj1.3700.network`. This included both non-encrypted and TLS-encrypted communication modes.
   - Test cases covered a wide range of word combinations from `word_list.txt`. 
   - The testing process ensured that the client adhered to the game protocol, correctly handled JSON data exchange with the server, and responded appropriately to the server's feedback for both correct and incorrect guesses.
        - Special attention was given to how the client handled the server's "retry" and "bye" messages, as well as processing the secret flags.

### Challenges Faced

During the development of the Wordle game client, two main challenges were encountered:

1. **Understanding and Implementing `TLS Handshake`**:
   - A significant challenge was gaining a thorough understanding of the TLS handshake process and its implementation over an established TCP connection. 
   - This involved comprehending the intricacies of SSL/TLS protocols and correctly using OpenSSL functions to secure the communication channel. 

2. **Handling JSON Parsing and Set Data Structures in C**:
   - Another challenge was related to the limitations of C in terms of `built-in high-level data structures` and `JSON parsing capabilities`.
   - While it was possible to code a custom JSON parser and create a set data structure from scratch, this approach was deemed inefficient compared to using existing libraries in other languages that offer built-in support.
   - To address this, cJSON library and a third-party set data structure were utilized. 

### Files Included
- `client.c`: Main client program source code.
- `client.h`: Header file for the client program, containing function declarations and necessary includes.
- `client_game.c`: Contains the game logic for the Wordle game client.
- `client_network.c`: Manages network communication, including socket handling and TLS functionality.
- `client_utils.c`: Utility functions used by the client program.
- `cJSON.c`: Source file for the cJSON library, used for JSON parsing and encoding.
- `cJSON.h`: cJSON library header file.
- `set.c`: Source file for the set data structure library.
- `set.h`: Header file for the set data structure library.
- `Makefile`: Used to compile and build the project.
- `word_list.txt`: List of valid words used for guessing in the game.

### Contributions and Acknowledgements
- cJSON library by Dave Gamble.
- Set library by Barrust.
- OpenSSL for TLS support.
