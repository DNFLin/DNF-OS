CC = gcc
CFLAGS = -I../include -Wall -Wextra -g
LDFLAGS = 

TESTS = test_dns.o test_socket.o
OBJECTS = ../src/custom_network_stack.o ../src/utils/dns_utils.o ../src/utils/socket_utils.o

all: $(TESTS)

test_dns: test_dns.o $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

test_socket: test_socket.o $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o

.PHONY: all clean