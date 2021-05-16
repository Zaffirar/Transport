#include "slidingWindow.h"
#include "socketAndServerHandling.h"

int lastACKReceived, lastSegmentWrittenToFile, numberOfSegments, socketFD, sizeOfFile;
struct sockaddr_in* serverAddress;
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
	clock_gettime(CLOCK_REALTIME, &segments[indexOfSegmnet].requestTime);
}

void setupSlidingWindow(int totalSize, struct in_addr* ipAddress, int port)
{
	lastACKReceived = -1;
	lastSegmentWrittenToFile = -1;
	sizeOfFile = totalSize;
	numberOfSegments = ceil(totalSize/MAX_REQUESTED_SEGMENT_SIZE);
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

int getSegmentSize(int numberOfSegmentInOrder)
{
	if(numberOfSegmentInOrder * MAX_REQUESTED_SEGMENT_SIZE < sizeOfFile)
	{
		return MAX_REQUESTED_SEGMENT_SIZE;
	}
	else
	{
		return sizeOfFile - numberOfSegmentInOrder * MAX_REQUESTED_SEGMENT_SIZE;
	}
}

char* createDownloadMesage(int from, int to)
{
	char* message;
	sprintf(message, 'GET %d %d\n', from, to);
	return message;
}

bool isTimedOut(int indexOfSegment) //TODO: check this
{
	struct timeval now;
	clock_gettime(CLOCK_REALTIME, &now);
    if (now.tv_usec < segments[indexOfSegment].requestTime.tv_usec)
    {
        now.tv_usec += 1000000;
        now.tv_sec--;
    }
    double passed = (now.tv_sec - segments[indexOfSegment].requestTime.tv_sec) 
		+ (now.tv_usec - segments[indexOfSegment].requestTime.tv_usec) * 0.000001;
    return passed > TIMEOUT_IN_MILISECONDS;
}

void requestForData()
{
	for(int i=0;i<WINDOW_SIZE;i++)
	{
		if(!segments[i].isDownloaded && isTimedOut(i))
		{
			int segmentInOrder = lastACKReceived + 1 + i;
			char* message = createDownloadMesage(
				segmentInOrder * MAX_REQUESTED_SEGMENT_SIZE,
				getSegmentSize(segmentInOrder));
			resetSegment(i);
			sendRequest(socketFD, serverAddress, message);
		}
	}
}

void downloadData() //TODO: write this
{
	// use select() and probably wait()
}

void slideWindowAndWriteDownloadedData(FILE* outputFile) //TODO: write this
{
	// this one is easy
}

int returnProgressOnDownladingData()
{
	int downloadedData = (lastACKReceived - 1) * MAX_REQUESTED_SEGMENT_SIZE
		+ getSegmentSize(lastACKReceived);

	return downloadedData > 0 ? downloadedData : 0;
}

void cleanupSlidingWindow()
{
	free(segments);
}