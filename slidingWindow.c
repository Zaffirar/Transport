// Wiktor Hamberger 308982
#include "slidingWindow.h"
#include "socketAndServerHandling.h"

int lastACKReceived, lastSegmentWrittenToFile, socketFD, sizeOfFile;
struct sockaddr_in serverAddress;
struct segment
{
	bool isDownloaded;
	struct timeval requestTime;
	char data[MAX_REQUESTED_SEGMENT_SIZE];
}*segments;

void resetSegment(int indexOfSegmnet)
{
	segments[indexOfSegmnet].isDownloaded = false;
	bzero(segments[indexOfSegmnet].data, MAX_REQUESTED_SEGMENT_SIZE);
	gettimeofday(&segments[indexOfSegmnet].requestTime, NULL);
}

int getSegmentSize(int numberOfSegmentInOrder)
{
	if(numberOfSegmentInOrder * MAX_REQUESTED_SEGMENT_SIZE < sizeOfFile)
	{
		return MAX_REQUESTED_SEGMENT_SIZE;
	}
	else
	{
		return sizeOfFile % MAX_REQUESTED_SEGMENT_SIZE;
	}
}

void setupSlidingWindow(int totalSize, struct in_addr* ipAddress, int port)
{
	lastACKReceived = -1;
	lastSegmentWrittenToFile = -1;
	sizeOfFile = totalSize;
	segments = malloc(sizeof(struct segment)*WINDOW_SIZE);
	if(segments == NULL)
	{
		fprintf(stderr, "Allocating memory error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(atexit(cleanupSlidingWindow) != 0)
	{
		fprintf(stderr, "Atexit error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	socketFD = createUDPSocket();
	serverAddress = createServerAddres(ipAddress, port);
	for(int i=0;i<WINDOW_SIZE;i++)
	{
		resetSegment(i);
	}
}

char* createDownloadMesage(int start, int length)
{
	static char message[20];
	if(sprintf(message, "GET %d %d\n", start, length) < 0)
	{
		fprintf(stderr, "Sprintf error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return message;
}

bool isTimedOut(int indexOfSegment)
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

void requestForData()
{
	for(int i=0;i<WINDOW_SIZE;i++)
	{
		if(!segments[i].isDownloaded && isTimedOut(i))
		{
			int segmentInOrder = lastACKReceived + 1 + i;
			if((segmentInOrder) * MAX_REQUESTED_SEGMENT_SIZE + getSegmentSize(segmentInOrder) > sizeOfFile ||
				getSegmentSize(segmentInOrder) == 0)
			{
				continue;
			}
			char* message = createDownloadMesage(
				segmentInOrder * MAX_REQUESTED_SEGMENT_SIZE,
				getSegmentSize(segmentInOrder));
			resetSegment(i);
			printf("Pytam! %s\n", message);
			sendRequest(socketFD, &serverAddress, message);
		}
	}
}

void parseResponse(char* responseMessage)
{
	int start, length;
	if(sscanf(responseMessage, "DATA %d %d", &start, &length) != 2)
	{
		fprintf(stderr, "Sscanf error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(start < lastACKReceived * MAX_REQUESTED_SEGMENT_SIZE ||
		start > (lastACKReceived + WINDOW_SIZE) * MAX_REQUESTED_SEGMENT_SIZE)
	{
		return;
	}
	printf("Mam odpowiedź! Start: %d długość: %d\n", start, length);
	int segmentIndex = (start/MAX_REQUESTED_SEGMENT_SIZE) - (lastACKReceived + 1);
	if(segments[segmentIndex].isDownloaded)
	{
		return;
	}
	segments[segmentIndex].isDownloaded=true;
	int i = 0;
	while(responseMessage[i] != '\n')
	{
		i++;
	}
	i++;
	memcpy(segments[segmentIndex].data, &responseMessage[i], length);
}

void downloadData()
{
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT_IN_MILISECONDS;
	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(socketFD, &descriptors);
	char* responseMessage;
	while(select(socketFD + 1, &descriptors, NULL, NULL, &timeout) > 0)
	{
		if(handleResponse(socketFD, serverAddress.sin_addr,
			serverAddress.sin_port, &responseMessage) == false)
		{
			continue;
		}
		parseResponse(responseMessage);
	}
}
void slideWindowAndWriteDownloadedData(FILE* outputFile)
{
	int i=0, j=0;
	while(segments[i].isDownloaded)
	{
		lastACKReceived++;
		size_t segmentSize=getSegmentSize(lastSegmentWrittenToFile+1);
		if(fwrite(segments[i].data, sizeof(char), segmentSize, outputFile) != segmentSize)
		{
			fprintf(stderr, "Write error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		lastSegmentWrittenToFile++;
		resetSegment(i);
		i++;
	}
	if(i == 0)
	{
		return;
	}
	while(i<WINDOW_SIZE)
	{
		segments[j]=segments[i];
		resetSegment(i);
		i++;
		j++;
	}
}

int returnProgressOnDownladingData()
{
	int downloadedData = (lastSegmentWrittenToFile - 1) * MAX_REQUESTED_SEGMENT_SIZE
		+ getSegmentSize(lastSegmentWrittenToFile);

	return downloadedData > 0 ? downloadedData : 0;
}

void cleanupSlidingWindow()
{
	free(segments);
}