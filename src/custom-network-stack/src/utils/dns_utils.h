#ifndef DNS_UTILS_H
#define DNS_UTILS_H

#include <stddef.h>

int send_dns_query(const char *hostname, const char *dns_server);
int parse_dns_response(unsigned char *response, size_t response_length, char *resolved_ip, size_t ip_size);

#endif // DNS_UTILS_H