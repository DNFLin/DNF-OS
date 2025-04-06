#ifndef CUSTOM_NETWORK_STACK_H
#define CUSTOM_NETWORK_STACK_H

#include <stddef.h>

// function declarations for the custom network stack
int custom_initialize_interface(const char *interface, const char *ip_address, const char *netmask, const char *gateway);
int custom_configure_dns(const char *primary_dns, const char *secondary_dns);
int custom_resolve_hostname(const char *hostname, char *resolved_ip, size_t ip_size);
int custom_start_server(const char *protocol, int port);

#endif // CUSTOM_NETWORK_STACK_H