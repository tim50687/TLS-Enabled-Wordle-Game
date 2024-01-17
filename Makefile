# Compiler and Compiler Flags
CC = cc
CFLAGS = -Wall -g

# Target Executable
TARGET = client

# Source File and Object Files
SRCS = client.c
OBJS = $(SRCS:.c=.o) # Turn every .c file in source to .o

all: $(TARGET)

client: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Remove the Executable and Object Files
clean: 
	rm -f $(TARGET) $(OBJS)