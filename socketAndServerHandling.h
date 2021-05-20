#ifndef SOCKETANDSERVERHANDLING
#define SOCKETANDSERVERHANDLING
#include "utilities.h"

int CreateUDPSocket();
struct sockaddr_in CreateServerAddres(struct in_addr* ip, short port);
void SendRequest(int sockFD, struct sockaddr_in* serverAddress, char* requestMessage);
int HandleResponse(int sockFD, struct in_addr expectedIp,
	in_port_t expectedPort, char** descinationOfResponse);
void CloseUDPSocket(int socketFD);

#endif
