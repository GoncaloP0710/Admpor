#ifndef CLIENT_H_GUARD
#define CLIENT_H_GUARD

#include "memory.h"
#include "main.h"
#include "synchronization.h"

void printStats (struct main_data* data);

void printNumStats();

int compareStats(char a, char b);

void printStartTimeStats(int i);

void printClientTimeStats(int i);

void printIntermediaryTimeStats(int i);

void printEnterpriseTimeStats(int i);

void printToatalTimeStats(int i);

void printClientIDStats(int i);

void printEnterpriseIDStats(int i);

void printIntermediaryIDStats(int i);

#endif