# Compiler and Compiler Flags
CC = cc
CFLAGS = -Wall -g

# Target Executable
TARGET = client

# Source File and Object Files
SRCS = client.c cJSON.c client_utils.c client_network.c client_game.c set.c
OBJS = $(SRCS:.c=.o) # Turn every .c file in source to .o

all: $(TARGET)

client: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Remove the Executable and Object Files
clean: 
	rm -f $(TARGET) $(OBJS)