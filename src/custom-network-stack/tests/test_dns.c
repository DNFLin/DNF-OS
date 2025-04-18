#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dns_utils.h"

void test_send_dns_query() {
    const char *hostname = "example.com";
    const char *dns_server = "8.8.8.8"; // Google's public DNS
    unsigned char response[512];

    int result = send_dns_query(hostname, dns_server, response, sizeof(response));
    if (result < 0) {
        printf("Test failed: send_dns_query returned an error\n");
    } else {
        printf("Test passed: send_dns_query executed successfully\n");
    }
}

void test_parse_dns_response() {
    unsigned char response[] = {
        // Sample DNS response data (this should be a valid DNS response)
    };
    size_t response_length = sizeof(response);

    char resolved_ip[INET_ADDRSTRLEN];
    int result = parse_dns_response(response, response_length, resolved_ip, sizeof(resolved_ip));
    if (result < 0) {
        printf("Test failed: parse_dns_response returned an error\n");
    } else {
        printf("Test passed: parse_dns_response executed successfully, resolved IP: %s\n", resolved_ip);
    }
}

int main() {
    printf("Running DNS utility tests...\n");
    test_send_dns_query();
    test_parse_dns_response();
    return 0;
}