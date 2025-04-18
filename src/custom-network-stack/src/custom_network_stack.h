#ifndef CUSTOM_NETWORK_STACK_H
#define CUSTOM_NETWORK_STACK_H

void custom_initialize_interface(const char *interface, const char *ip_address, const char *netmask, const char *gateway);
void custom_configure_dns(const char *primary_dns, const char *secondary_dns);
int custom_resolve_hostname(const char *hostname, char *resolved_ip, size_t ip_size);
void custom_start_server(const char *protocol, int port);
void send_dns_query(const char *hostname, const char *dns_server);

#endif // CUSTOM_NETWORK_STACK_H