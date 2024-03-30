#include "main.h"
#include "synchronization.h"
#include "memory.h"

#include<stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

pid_t mainPid;

struct main_data* dataGlobalS;
struct comm_buffers* buffersGlobalS;
struct semaphores* semsGlobalS;
int pid_Main;

struct main_data* dataGlobalSAlarm;
int alarm_timeAlarm;

int compare(char a, char b) {
    if (a == b)
        return 0;
    else
        return 1;
}

void printStartTime(int i) {
    struct timespec ts = dataGlobalSAlarm->results[i].start_time;
    printf("start_time: %ld", ts.tv_sec);
}

void printClientTime(int i) {
    struct timespec tsC = dataGlobalSAlarm->results[i].client_time;
    printf(" client:");
    printf("%d", dataGlobalSAlarm->results[i].requesting_client);
    printf(" client_time: %ld", tsC.tv_sec);
}

void printIntermediaryTime(int i) {
    struct timespec tsI = dataGlobalSAlarm->results[i].intermed_time;
    printf(" intermediary:");
    printf("%d", dataGlobalSAlarm->results[i].receiving_interm);
    printf(" intermediary_time: %ld", tsI.tv_sec);
}

void printEnterpriseTime(int i) {
    struct timespec tsE = dataGlobalSAlarm->results[i].enterp_time;
    printf(" enterprise:");
    printf("%d", dataGlobalSAlarm->results[i].requested_enterp);
    printf(" enterprise_time: %ld", tsE.tv_sec);
}

void sig_handler() {
    struct main_data* data = dataGlobalSAlarm;
    for (size_t i = 0; i < MAX_RESULTS ; i++) {
        if(data->results[i].client_time.tv_sec != 0) {
            if (compare(data->results[i].status, 'M') == 0) {
                printf("op:");
                printf("%d", dataGlobalSAlarm->results[i].id);
                printf(" status:M ");
                printStartTime(i);
                printf("\n");
            }else if (compare(data->results[i].status, 'C') == 0) {
                printf("op:");
                printf("%d", dataGlobalSAlarm->results[i].id);
                printf(" status:C ");
                printStartTime(i);
                printClientTime(i);
                printf("\n");
            }else if (compare(data->results[i].status, 'I') == 0) {
                printf("op:");
                printf("%d", dataGlobalSAlarm->results[i].id);
                printf(" status:I ");
                printStartTime(i);
                printClientTime(i);
                printIntermediaryTime(i);
                printf("\n");
            }else if (compare(data->results[i].status, 'A') == 0) {
                printf("op:");
                printf("%d", dataGlobalSAlarm->results[i].id);
                printf(" status:A ");
                printStartTime(i);
                printClientTime(i);
                printIntermediaryTime(i);
                printEnterpriseTime(i);
                printf("\n");
            }else {//'E'
                printf("op:");
                printf("%d", dataGlobalSAlarm->results[i].id);
                printf(" status:E ");
                printStartTime(i);
                printClientTime(i);
                printIntermediaryTime(i);
                printEnterpriseTime(i);
                printf("\n");
            }
        }
    }
    alarm(alarm_timeAlarm);
}

void mainAlarm(struct main_data* data, int alarm_time) {
    dataGlobalSAlarm = data;
    alarm_timeAlarm = alarm_time;

    int pid = fork();
    if (pid == -1) { /* Houve algum erro */
        perror("Error launching client");
        exit(1);
    }
    if (pid == 0) { /* Processo filho */
        signal(SIGALRM, sig_handler);
        alarm(alarm_time);

        while (1) {
            if (*data -> terminate == 1) {
                exit(1);
            }
            pause();
        }
    } else { /* Processo pai */
        
    }
}

void handle_sigint1(int sig) {
    signal(SIGINT, handle_sigint1);
    stop_execution(dataGlobalS, buffersGlobalS, semsGlobalS);
    exit(0);
}

void handle_sigint2(int sig) {
    signal(SIGINT, handle_sigint2);
}

void mainSignal(int pid) {
    if (pid == pid_Main) {
        signal(SIGINT, handle_sigint1);
    } else {
        signal(SIGINT, handle_sigint2);
    }
}

void setInfo (struct main_data* data, struct comm_buffers* buffers, struct semaphores* sems, int pid) {
    dataGlobalS = data;
    buffersGlobalS = buffers;
    semsGlobalS = sems;
    pid_Main = pid;
}