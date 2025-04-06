#include "custom_network_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

// Custom function to initialize a network interface
int custom_initialize_interface(const char *interface, const char *ip_address, const char *netmask, const char *gateway) {
    printf("Initializing interface: %s\n", interface);
    printf("IP Address: %s\n", ip_address);
    printf("Netmask: %s\n", netmask);
    printf("Gateway: %s\n", gateway);

    // Simulate success
    return 0;
}

// Custom function to configure DNS
int custom_configure_dns(const char *primary_dns, const char *secondary_dns) {
    printf("Configuring DNS servers:\n");
    printf("Primary DNS: %s\n", primary_dns);
    printf("Secondary DNS: %s\n", secondary_dns);

    // Simulate success
    return 0;
}

// Custom function to resolve a hostname dynamically
int custom_resolve_hostname(const char *hostname, char *resolved_ip, size_t ip_size) {
    printf("Resolving hostname: %s\n", hostname);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "DNS resolution failed for %s: %s\n", hostname, gai_strerror(status));
        return -1;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &addr->sin_addr, resolved_ip, ip_size);
    printf("Resolved %s to %s\n", hostname, resolved_ip);

    freeaddrinfo(res);
    return 0;
}

// Custom function to start a TCP/UDP server
int custom_start_server(const char *protocol, int port) {
    printf("Starting %s server on port %d\n", protocol, port);

    int sockfd;
    struct sockaddr_in server_addr;

    if (strcmp(protocol, "TCP") == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    } else if (strcmp(protocol, "UDP") == 0) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    } else {
        fprintf(stderr, "Unsupported protocol: %s\n", protocol);
        return -1;
    }

    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return -1;
    }

    if (strcmp(protocol, "TCP") == 0) {
        if (listen(sockfd, 5) < 0) {
            perror("Listen failed");
            close(sockfd);
            return -1;
        }
        printf("TCP server listening on port %d\n", port);
    } else {
        printf("UDP server listening on port %d\n", port);
    }

    close(sockfd);
    return 0;
}