#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include "aptime.h"

// Function to get current time as timespec struct
void writeLog(char* filename, char* toWrite, struct timespec* time);

#endif