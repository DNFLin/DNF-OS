CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = src/custom_network_stack.c src/utils/dns_utils.c src/utils/socket_utils.c
OBJ = $(SRC:.c=.o)
TARGET = custom_network_stack

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean