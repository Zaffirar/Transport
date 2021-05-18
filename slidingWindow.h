#ifndef SLIDINGWINDOW
#define SLIDINGWINDOW

#include "utilities.h"
#include "socketAndServerHandling.h"

void resetSegment(int indexOfSegmnet);
int getSegmentSize(int numberOfSegmentInOrder);
void setupSlidingWindow(int totalSize, struct in_addr* ipAddress, int port);
char* createDownloadMesage(int from, int to);
bool isTimedOut(int indexOfSegment);
void requestForData();
void parseResponse(char* responseMessage);
void downloadData();
void slideWindowAndWriteDownloadedData(FILE* outputFile);
int returnProgressOnDownladingData();
void cleanupSlidingWindow();

#endif
