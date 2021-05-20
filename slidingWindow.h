#ifndef SLIDINGWINDOW
#define SLIDINGWINDOW

#include "utilities.h"
#include "socketAndServerHandling.h"

void ResetSegment(int indexOfSegmnet);
int GetSegmentSize(int numberOfSegmentInOrder);
void SetupSlidingWindow(int totalSize, struct in_addr* ipAddress, int port);
char* CreateDownloadMesage(int from, int to);
bool IsTimedOut(int indexOfSegment);
void RequestForData();
void ParseResponse(char* responseMessage, int responseMessageLength);
void DownloadData();
void SlideWindowAndWriteDownloadedData(FILE* outputFile);
int ReturnProgressOnDownladingData();
void CleanupSlidingWindow();

#endif
