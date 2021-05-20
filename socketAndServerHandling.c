// Wiktor Hamberger 308982
#include "socketAndServerHandling.h"

int CreateUDPSocket()
{
	int socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketFD == -1)  {
		fprintf(stderr, "Creating socket error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return socketFD;
}

struct sockaddr_in CreateServerAddres(struct in_addr* ip, int port)
{
	struct sockaddr_in serverAddress;
	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr = *ip;
	return serverAddress;
}

void SendRequest(int socketFD, struct sockaddr_in* serverAddress, char* requestMessage)
{
	ssize_t messageLength = strlen(requestMessage);
	if (sendto(socketFD, requestMessage, messageLength, 0, 
		(struct sockaddr*)serverAddress, sizeof(*serverAddress)) == -1)
	{
		fprintf(stderr, "Sending request error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int HandleResponse(int socketFD, struct in_addr expectedIp, 
	in_port_t expectedPort, char** descinationOfResponse)
{
	struct sockaddr_in senderAddress;
	socklen_t senderAddressLength = sizeof(senderAddress);
	static char responseBuffer[MAX_RESPONSE_SIZE];
	int receivedBytes = recvfrom(socketFD, responseBuffer, MAX_RESPONSE_SIZE, 0, 
		(struct sockaddr*)&senderAddress, &senderAddressLength);
	if(receivedBytes == -1)
	{
		fprintf(stderr, "Receving response error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(senderAddress.sin_addr.s_addr != expectedIp.s_addr || 
		senderAddress.sin_port != expectedPort)
	{
		return 0;
	}
	*descinationOfResponse = responseBuffer;
	return receivedBytes;
}

void CloseUDPSocket(int socketFD)
{
	close(socketFD);
}
