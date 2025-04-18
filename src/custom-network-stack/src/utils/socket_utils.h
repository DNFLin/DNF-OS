#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Function to create a socket
int create_socket(int domain, int type, int protocol);

// Function to bind a socket to an address
int bind_socket(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// Function to listen for incoming connections (for TCP sockets)
int listen_socket(int sockfd, int backlog);

// Function to accept a connection on a socket (for TCP sockets)
int accept_connection(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// Function to connect to a server
int connect_to_server(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// Function to set socket options
int set_socket_option(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

#endif // SOCKET_UTILS_H