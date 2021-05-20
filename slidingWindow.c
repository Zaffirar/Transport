// Wiktor Hamberger 308982
#include "slidingWindow.h"
#include "socketAndServerHandling.h"

int lastACKReceived, socketFD, sizeOfFile;
struct sockaddr_in serverAddress;
struct segment
{
	bool isDownloaded;
	struct timeval requestTime;
	char data[MAX_REQUESTED_SEGMENT_SIZE];
}*segments;

void ResetSegment(int indexOfSegmnet)
{
	segments[indexOfSegmnet].isDownloaded = false;
	bzero(segments[indexOfSegmnet].data, MAX_REQUESTED_SEGMENT_SIZE);
	gettimeofday(&segments[indexOfSegmnet].requestTime, NULL);
}

int GetSegmentSize(int numberOfSegmentInOrder)
{
	if((numberOfSegmentInOrder + 1) * MAX_REQUESTED_SEGMENT_SIZE <= sizeOfFile)
	{
		return MAX_REQUESTED_SEGMENT_SIZE;
	}
	else
	{
		return sizeOfFile % MAX_REQUESTED_SEGMENT_SIZE;
	}
}

void SetupSlidingWindow(int totalSize, struct in_addr* ipAddress, int port)
{
	lastACKReceived = -1;
	sizeOfFile = totalSize;
	segments = malloc(sizeof(struct segment)*WINDOW_SIZE);
	if(segments == NULL)
	{
		fprintf(stderr, "Allocating memory error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(atexit(CleanupSlidingWindow) != 0)
	{
		fprintf(stderr, "Atexit error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	socketFD = CreateUDPSocket();
	serverAddress = CreateServerAddres(ipAddress, port);
	for(int i=0;i<WINDOW_SIZE;i++)
	{
		ResetSegment(i);
	}
}

char* CreateDownloadMesage(int start, int length)
{
	static char message[20];
	if(sprintf(message, "GET %d %d\n", start, length) < 0)
	{
		fprintf(stderr, "Sprintf error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return message;
}

bool IsTimedOut(int indexOfSegment)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	if (now.tv_usec < segments[indexOfSegment].requestTime.tv_usec)
	{
		now.tv_usec += 1000;
		now.tv_sec--;
	}
	double timePassed = (now.tv_sec - segments[indexOfSegment].requestTime.tv_sec) * 1000
		+ (now.tv_usec - segments[indexOfSegment].requestTime.tv_usec);
	return timePassed > TIMEOUT_IN_MILISECONDS;
}

void RequestForData()
{
	for(int i=0;i<WINDOW_SIZE;i++)
	{
		if(!segments[i].isDownloaded && IsTimedOut(i))
		{
			int segmentInOrder = lastACKReceived + 1 + i;
			if(segmentInOrder * MAX_REQUESTED_SEGMENT_SIZE + 
				GetSegmentSize(segmentInOrder) > sizeOfFile ||
				GetSegmentSize(segmentInOrder) == 0)
			{
				continue;
			}
			char* message = CreateDownloadMesage(
				segmentInOrder * MAX_REQUESTED_SEGMENT_SIZE, 
				GetSegmentSize(segmentInOrder));
			ResetSegment(i);
			SendRequest(socketFD, &serverAddress, message);
		}
	}
}

void ParseResponse(char* responseMessage, int responseMessageLength)
{
	int start, length;
	if(sscanf(responseMessage, "DATA %d %d", &start, &length) != 2)
	{
		fprintf(stderr, "Sscanf error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(start <= lastACKReceived * MAX_REQUESTED_SEGMENT_SIZE ||
		start > (lastACKReceived + WINDOW_SIZE) * MAX_REQUESTED_SEGMENT_SIZE)
	{
		return;
	}
	int segmentIndex = (start/MAX_REQUESTED_SEGMENT_SIZE) - (lastACKReceived + 1);
	if(segments[segmentIndex].isDownloaded)
	{
		return;
	}
	segments[segmentIndex].isDownloaded=true;
	int i = responseMessageLength - length;
	memcpy(segments[segmentIndex].data, &responseMessage[i], length);
}

void DownloadData()
{
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT_IN_MILISECONDS*3;
	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(socketFD, &descriptors);
	char* responseMessage;
	int messageLength;
	while(select(socketFD + 1, &descriptors, NULL, NULL, &timeout) > 0)
	{
		messageLength = HandleResponse(socketFD, serverAddress.sin_addr, 
			serverAddress.sin_port, &responseMessage);
		if(messageLength == 0)
		{
			continue;
		}
		ParseResponse(responseMessage, messageLength);
	}
}
void SlideWindowAndWriteDownloadedData(FILE* outputFile)
{
	int i=0, j=0;
	while(i<WINDOW_SIZE && segments[i].isDownloaded)
	{
		lastACKReceived++;
		size_t segmentSize=GetSegmentSize(lastACKReceived);
		if(fwrite(segments[i].data, sizeof(char), segmentSize, outputFile) != segmentSize)
		{
			fprintf(stderr, "Write error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		ResetSegment(i);
		i++;
	}
	if(i == 0)
	{
		return;
	}
	while(i<WINDOW_SIZE)
	{
		segments[j]=segments[i];
		ResetSegment(i);
		i++;
		j++;
	}
	printf("Downloaded %d from %d bytes.\n", ReturnProgressOnDownladingData(), sizeOfFile);
}

int ReturnProgressOnDownladingData()
{
	int downloadedData = lastACKReceived * MAX_REQUESTED_SEGMENT_SIZE
		+ GetSegmentSize(lastACKReceived);
	return downloadedData > 0 ? downloadedData : 0;
}

void CleanupSlidingWindow()
{
	free(segments);
}