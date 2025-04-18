#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Common definitions and constants
#define MAX_BUFFER_SIZE 1024
#define MAX_HOSTNAME_LENGTH 256

// Utility function prototypes
void handle_error(const char *msg);

#endif // COMMON_H