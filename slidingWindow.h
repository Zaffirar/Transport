#ifndef SLIDINGWINDOW
#define SLIDINGWINDOW

#include "utilities.h"
#include "socketAndServerHandling.h"

void SetupSlidingWindow(int totalSize, struct in_addr* ipAddress, short port);
void RequestForData(void);
void DownloadData(void);
void SlideWindowAndWriteDownloadedData(FILE* outputFile);
int ReturnProgressOnDownladingData(void);
void CleanupSlidingWindow(void);

#endif
