#include "dns_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DNS_QUERY_ID 0x1234

// Function to create a DNS query
unsigned char* create_dns_query(const char *hostname, size_t *query_length) {
    unsigned char *query = (unsigned char *)malloc(256);
    if (!query) {
        perror("Failed to allocate memory for DNS query");
        return NULL;
    }
    memset(query, 0, 256);

    query[0] = (DNS_QUERY_ID >> 8) & 0xFF; // Transaction ID
    query[1] = DNS_QUERY_ID & 0xFF;
    query[2] = 0x01; // Standard query
    query[5] = 0x01; // One question

    char *qname = (char *)&query[12];
    const char *label = hostname;
    while (*label) {
        const char *dot = strchr(label, '.');
        if (!dot) dot = label + strlen(label);
        *qname++ = dot - label;
        memcpy(qname, label, dot - label);
        qname += dot - label;
        label = (*dot) ? dot + 1 : dot;
    }
    *qname++ = 0; // Null-terminate the QNAME
    query[qname - query] = 0x00; // QTYPE A
    query[qname - query + 1] = 0x01; // QCLASS IN

    *query_length = qname - query + 2; // Length of the query
    return query;
}

// Function to parse the DNS response
int parse_dns_response(unsigned char *response, size_t response_length, char *resolved_ip) {
    if (response_length < 12) {
        fprintf(stderr, "Invalid DNS response length\n");
        return -1;
    }

    // Check if the response is valid
    if (response[3] & 0x0F) {
        fprintf(stderr, "DNS response indicates an error\n");
        return -1;
    }

    // Skip the header (12 bytes)
    int answer_count = (response[6] << 8) | response[7];
    unsigned char *ptr = response + 12;

    // Skip the question section
    while (*ptr) {
        ptr += *ptr + 1;
    }
    ptr += 5; // Skip the null byte and QTYPE, QCLASS

    // Parse the answer section
    for (int i = 0; i < answer_count; i++) {
        if (ptr + 10 > response + response_length) {
            fprintf(stderr, "Invalid DNS response format\n");
            return -1;
        }

        // Skip the name
        if (*ptr & 0xC0) {
            ptr += 2; // Pointer to the name
        } else {
            while (*ptr) {
                ptr += *ptr + 1;
            }
            ptr++;
        }

        // Check the type
        if (ptr + 8 > response + response_length) {
            fprintf(stderr, "Invalid DNS response format\n");
            return -1;
        }
        uint16_t type = (ptr[0] << 8) | ptr[1];
        uint16_t data_length = (ptr[8] << 8) | ptr[9];

        // If the answer is of type A
        if (type == 1) {
            if (ptr + 10 + data_length > response + response_length) {
                fprintf(stderr, "Invalid DNS response format\n");
                return -1;
            }
            struct in_addr addr;
            memcpy(&addr, ptr + 10, sizeof(addr));
            strcpy(resolved_ip, inet_ntoa(addr));
            return 0;
        }

        ptr += 10 + data_length; // Move to the next answer
    }

    fprintf(stderr, "No valid A record found in DNS response\n");
    return -1;
}