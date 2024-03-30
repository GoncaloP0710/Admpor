#ifndef APSIGNAL_H_GUARD
#define APSIGNAL_H_GUARD

#include "main.h"
#include "synchronization.h"
#include "memory.h"

void setMainPid(pid_t pid);

void mainSignal(int pid);

void mainAlarm(struct main_data* data, int alarm_time);

void handle_alarm(int sig);

void printStartTime(int i);

void printClientTime(int i);

void printIntermediaryTime(int i);

void printEnterpriseTime(int i);

void handle_sigint1(int sig);

void handle_sigint2(int sig);

void setInfo (struct main_data* data, struct comm_buffers* buffers, struct semaphores* sems, int pid);

#endif