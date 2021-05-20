#ifndef UTILITIES
#define UTILITIES


#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILE_SIZE 10000000
#define MAX_REQEST_MESSAGE_LENGTH 20
#define MAX_REQUESTED_SEGMENT_SIZE 1000
#define MAX_RESPONSE_SIZE 1100
#define SEGMENT_TIMEOUT 800
#define SOCKET_TIMEOUT 2000
#define WINDOW_SIZE 3500

#endif
