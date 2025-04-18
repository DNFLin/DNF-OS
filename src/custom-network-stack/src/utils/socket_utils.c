#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int create_socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }
    return sockfd;
}

int bind_socket(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) < 0) {
        perror("Bind failed");
        return -1;
    }
    return 0;
}

int listen_socket(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        perror("Listen failed");
        return -1;
    }
    return 0;
}

int accept_connection(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int new_sockfd = accept(sockfd, addr, addrlen);
    if (new_sockfd < 0) {
        perror("Accept failed");
        return -1;
    }
    return new_sockfd;
}

void close_socket(int sockfd) {
    if (close(sockfd) < 0) {
        perror("Close failed");
    }
}