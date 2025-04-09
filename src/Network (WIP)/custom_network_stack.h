#ifndef CUSTOM_NETWORK_STACK_H
#define CUSTOM_NETWORK_STACK_H

#include <stddef.h>

// Enable a network interface
void custom_initialize_interface(const char *interface, const char *ip_address, const char *netmask, const char *gateway);

// Config DNS servers
void custom_configure_dns(const char *primary_dns, const char *secondary_dns);

// Resolve a hostname to an IP address
int custom_resolve_hostname(const char *hostname, char *resolved_ip, size_t ip_size);

// Start a TCP or UDP server
void custom_start_server(const char *protocol, int port);

#endif // CUSTOM_NETWORK_STACK_H