#ifndef SLIDINGWINDOW
#define SLIDINGWINDOW

#include "utilities.h"
#include "socketAndServerHandling.h"

void resetSegment(int indexOfSegmnet);
void setupSlidingWindow(int totalSize, struct in_addr* ipAddress, int port);
int getSegmentSize(int numberOfSegmentInOrder);
char* createDownloadMesage(int from, int to);
void requestForData();
void downloadData();
void slideWindowAndWriteDownloadedData(FILE* outputFile);
int returnProgressOnDownladingData();
void cleanupSlidingWindow();

#endif
