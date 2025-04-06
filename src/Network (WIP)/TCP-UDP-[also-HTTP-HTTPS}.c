#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

void configure_network(const char *interface, const char *ip_address, const char *netmask, const char *gateway) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    // set IP address
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

    // set netmask
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

    // bring interface up
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

    // set default gateway
    struct rtentry route;
    memset(&route, 0, sizeof(route));
    struct sockaddr_in *gw = (struct sockaddr_in *)&route.rt_gateway;
    struct sockaddr_in *dst = (struct sockaddr_in *)&route.rt_dst;
    struct sockaddr_in *mask = (struct sockaddr_in *)&route.rt_genmask;

    gw->sin_family = AF_INET;
    if (inet_pton(AF_INET, gateway, &gw->sin_addr) <= 0) {
        perror("Invalid gateway");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    dst->sin_family = AF_INET;
    dst->sin_addr.s_addr = INADDR_ANY;

    mask->sin_family = AF_INET;
    mask->sin_addr.s_addr = INADDR_ANY;

    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_dev = (char *)interface;

    if (ioctl(sockfd, SIOCADDRT, &route) < 0) {
        perror("Failed to set default gateway");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Network interface %s configured with IP %s, netmask %s, and gateway %s\n", interface, ip_address, netmask, gateway);
    close(sockfd);
}

void configure_dns() {
    FILE *resolv = fopen("/etc/resolv.conf", "w");
    if (!resolv) {
        perror("Failed to open /etc/resolv.conf");
        exit(EXIT_FAILURE);
    }

    fprintf(resolv, "nameserver 1.1.1.1\n");
    fprintf(resolv, "nameserver 1.0.0.1\n");
    fclose(resolv);

    printf("DNS servers configured: 1.1.1.1, 1.0.0.1\n");
}

void query_dns(const char *hostname) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "DNS resolution failed: %s\n", gai_strerror(status));
        return;
    }

    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
    printf("Resolved %s to %s\n", hostname, ip);

    freeaddrinfo(res);
}

void tcp_udp_server(const char *protocol, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    if (strcmp(protocol, "TCP") == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    } else if (strcmp(protocol, "UDP") == 0) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    } else {
        fprintf(stderr, "Unsupported protocol: %s\n", protocol);
        return;
    }

    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

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

void setup_on_boot() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // parent process exits
        exit(EXIT_SUCCESS);
    }

    // child process continues
    if (setsid() < 0) {
        perror("Failed to create new session");
        exit(EXIT_FAILURE);
    }

    // redirect file descriptors to /dev/null
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/null");
        exit(EXIT_FAILURE);
    }
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > STDERR_FILENO) {
        close(fd);
    }

    const char *interface = "eth0";
    const char *ip_address = "192.168.1.100";
    const char *netmask = "255.255.255.0";
    const char *gateway = "192.168.1.1";

    configure_network(interface, ip_address, netmask, gateway);
    configure_dns(); // set DNS servers

    const char *hostname = "example.com";
    query_dns(hostname);

    tcp_udp_server("TCP", 8080);
    tcp_udp_server("UDP", 8080);

    while (1) {
        sleep(60); // keep the process running
    }
}

int main() {
    setup_on_boot();
    return 0;
}
