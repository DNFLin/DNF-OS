#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket_utils.h"

void test_create_socket() {
    int sockfd = create_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully: %d\n", sockfd);
    close(sockfd);
}

void test_bind_socket() {
    int sockfd = create_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        return;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind_socket(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Failed to bind socket\n");
        close(sockfd);
        return;
    }
    printf("Socket bound successfully\n");
    close(sockfd);
}

void run_socket_tests() {
    printf("Running socket utility tests...\n");
    test_create_socket();
    test_bind_socket();
    printf("All socket utility tests completed.\n");
}

int main() {
    run_socket_tests();
    return 0;
}