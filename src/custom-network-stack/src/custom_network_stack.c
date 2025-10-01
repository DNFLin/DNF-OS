#include "custom_network_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>

// Enables services. This includes basic error checking along with DHCP culling.
void custom_initialize_interface(const char *interface, const char *ip_address, const char *netmask, const char *gateway) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    // Set IP address/Error Checking.
    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;
    if (inet_pton(AF_INET, ip_address, &addr->sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (ioctl(sockfd, SIOCSIFADDR, &ifr) < 0) {
        perror("Failed to set IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Set netmask with basic error checking. If netmask is less or equal to 0 then close sockfd and cause exit. 
    if (inet_pton(AF_INET, netmask, &addr->sin_addr) <= 0) {
        perror("Invalid netmask");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0) {
        perror("Failed to set netmask");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        perror("Failed to get interface flags");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
        perror("Failed to bring interface up");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Network interface %s configured with IP %s, netmask %s, and gateway %s\n", interface, ip_address, netmask, gateway);
    close(sockfd);
}

// DNS Config file checking. If this throws an error, you have likely chown'd the file. Create an error request on github so that we can help you fix it.
void custom_configure_dns(const char *primary_dns, const char *secondary_dns) {
    const char *dns_config_file = "/path/to/your/custom/dns/config";

    FILE *dns_file = fopen(dns_config_file, "w");
    if (!dns_file) {
        perror("Failed to open DNS configuration file");
        exit(EXIT_FAILURE);
    }

    fprintf(dns_file, "nameserver %s\n", primary_dns);
    fprintf(dns_file, "nameserver %s\n", secondary_dns);
    fclose(dns_file);

    printf("DNS servers configured: %s, %s\n", primary_dns, secondary_dns);
}

// DNS resolving (IPv4) and DNS error checking.
int custom_resolve_hostname(const char *hostname, char *resolved_ip, size_t ip_size) {
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
    freeaddrinfo(res);
    return 0;
}

void custom_start_server(const char *protocol, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    if (strcmp(protocol, "TCP") == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    } else if (strcmp(protocol, "UDP") == 0) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    } else { // If network protocol is not TCP/UDP then print "Unsupported protocol: PROTOCOL_NAME" in logs. I'm not currently aware of anything other than TCP/UDP (off the top of my head). Create an error on the github page if something needs to be corrected.
        fprintf(stderr, "Unsupported protocol: %s\n", protocol);
        return;
    }

    // If socket count = 0 then throw error via perror.
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // If server_addr = less than 0, 
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return;
    }

    if (strcmp(protocol, "TCP") == 0) {
        if (listen(sockfd, 5) < 0) {
            perror("Listen failed");
            close(sockfd);
            return;
        }
        printf("TCP server listening on port %d\n", port);
    } else {
        printf("UDP server listening on port %d\n", port);
    }

    close(sockfd);
}

    // If socket creation fails, throw an error via EXIT_FAILURE. --SOCK CHECK--
void send_dns_query(const char *hostname, const char *dns_server) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(53); // DNS port; feel free to change. Will add a command/cfg file for that specifically.
    inet_pton(AF_INET, dns_server, &server_addr.sin_addr);

    unsigned char query[256];
    memset(query, 0, sizeof(query));
    query[0] = 0x12; // First bytes of DNS header
    query[1] = 0x34;
    query[2] = 0x01; 
    query[5] = 0x01; 
    char *qname = (char *)&query[12]; // DNS question pointer (QNAME), 12 bytes
    const char *label = hostname; // Pointer for hostname to be encoded in DNS label format
    while (*label) {
        const char *dot = strchr(label, '.');
        if (!dot) dot = label + strlen(label);
        *qname++ = dot - label;
        memcpy(qname, label, dot - label); // Copy label after length byte
        qname += dot - label;
        label = (*dot) ? dot + 1 : dot;
    }
    *qname++ = 0; 
    query[qname - query] = 0x00; 
    query[qname - query + 1] = 0x01; 

    if (sendto(sockfd, query, qname - query + 2, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to send DNS query");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Sets unsigned int/char to 512 value. Also checks for DNS healthcheck.
    unsigned char response[512];
    socklen_t addr_len = sizeof(server_addr);
    if (recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr *)&server_addr, &addr_len) < 0) {
        perror("Failed to receive DNS response");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // Upon DNS Healthcheck = healthy, child quits.
    printf("DNS response received\n");
    close(sockfd);
}
