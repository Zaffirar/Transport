#ifndef SOCKETANDSERVERHANDLING
#define SOCKETANDSERVERHANDLING
#include "utilities.h"

int createUDPSocket();
struct sockaddr_in createServerAddres(struct in_addr* ip, int port);
void sendRequest(int sockFD, struct sockaddr_in* serverAddress, char* requestMessage);
bool handleResponse(int sockFD, struct in_addr expectedIp, 
                    in_port_t expectedPort, char** descinationOfResponse);
void closeUDPSocket(int socketFD);

#endif
