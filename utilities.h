#ifndef UTILITIES
#define UTILITIES

#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MAX_REQUESTED_SEGMENT_SIZE 100
#define MAX_RESPONSE_SIZE 1100
#define WINDOW_SIZE 4
#define TIMEOUT_IN_MILISECONDS 2000

#endif
