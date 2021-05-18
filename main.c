#include "utilities.h"
#include "slidingWindow.h"

void downloadFile(struct in_addr* ipAddress, int port, FILE* outputFile, int totalSize)
{
	int downloadedData = 0;
	setupSlidingWindow(totalSize, ipAddress, port);
	while(downloadedData < totalSize)
	{
		requestForData();
		downloadData();
		slideWindowAndWriteDownloadedData(outputFile);
		downloadedData = returnProgressOnDownladingData();
		printf("Downloaded %d from %d bytes.\n", downloadedData, totalSize);
	}
	cleanupSlidingWindow();
}

int main(int argc, char* argv[])
{
	struct in_addr ipAddress;
	int port;
	FILE* outputFile;
	int totalSize;
	if(argc != 5)
	{
		fprintf(stderr, 
			"Usage: ./transport [ip address] [port] [output file name] [number of bytes to be downloaded]\n");
		return EXIT_FAILURE;
	}
	if(inet_pton(AF_INET, argv[1], &ipAddress) != 1)
	{
		fprintf(stderr, "Parsing ip addres failed.");
		return EXIT_FAILURE;
	}
	port = atoi(argv[2]);
	outputFile = fopen(argv[3], "w");
	if(outputFile == NULL)
	{
		fprintf(stderr, "Output file error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	totalSize = atoi(argv[4]);
	downloadFile(&ipAddress, port, outputFile, totalSize);
	fclose(outputFile);
	return EXIT_SUCCESS;
}
