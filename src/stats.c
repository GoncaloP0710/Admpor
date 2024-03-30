#include "stats.h"
#include "main.h"
#include "memory.h"
#include "synchronization.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


FILE *statFile;
struct main_data* dataGlobal;

void printStats (struct main_data* data) {
    FILE *txtfile = fopen(data->statistics_filename, "w");
    if (txtfile == NULL){
        perror("Ficheiro de Log.");
        exit(1);
    }
    statFile = txtfile;
    //fseek(txtfile, 0, SEEK_END);
    dataGlobal = data;
    fprintf(statFile, "Process Statistics:\n");
    printNumStats();
    fprintf(statFile, "\n");
    for (size_t i = 0; i < MAX_RESULTS ; i++) {
        if(data->results[i].client_time.tv_sec != 0) {
            if (compareStats(data->results[i].status, 'M') == 0) {
                fprintf(statFile, "Request:");
                fprintf(statFile, "%d", data->results[i].id);
                fprintf(statFile, "\n");
                fprintf(statFile, "Status: M ");
                fprintf(statFile, "\n");
                printStartTimeStats(i);
                fprintf(statFile, "\n");
                fprintf(statFile, "\n");
            }else if (compareStats(data->results[i].status, 'C') == 0) {
                fprintf(statFile,"Request:");
                fprintf(statFile,"%d", data->results[i].id);
                fprintf(statFile,"\n");
                fprintf(statFile,"Status: C ");
                fprintf(statFile,"\n");
                printClientIDStats(i);
                printStartTimeStats(i);
                printClientTimeStats(i);
                fprintf(statFile,"\n");
                fprintf(statFile,"\n");
            }else if (compareStats(data->results[i].status, 'I') == 0) {
                fprintf(statFile,"Request:");
                fprintf(statFile,"%d", data->results[i].id);
                fprintf(statFile,"\n");
                fprintf(statFile,"Status: I ");
                fprintf(statFile,"\n");
                printClientIDStats(i);
                printIntermediaryIDStats(i);
                printStartTimeStats(i);
                printClientTimeStats(i);
                printIntermediaryTimeStats(i);
                fprintf(statFile,"\n");
                fprintf(statFile,"\n");
            }else if (compareStats(data->results[i].status, 'A') == 0) {
                fprintf(statFile,"Request:");
                fprintf(statFile,"%d", data->results[i].id);
                fprintf(statFile,"\n");
                fprintf(statFile,"Status: A ");
                fprintf(statFile,"\n");
                printClientIDStats(i);
                printIntermediaryIDStats(i);
                printEnterpriseIDStats(i);
                printStartTimeStats(i);
                printClientTimeStats(i);
                printIntermediaryTimeStats(i);
                printEnterpriseTimeStats(i);
                printToatalTimeStats(i);
                fprintf(statFile,"\n");
            }else {//'E'
                fprintf(statFile,"Request:");
                fprintf(statFile,"%d", data->results[i].id);
                fprintf(statFile,"\n");
                fprintf(statFile,"Status: E ");
                fprintf(statFile,"\n");
                printClientIDStats(i);
                printIntermediaryIDStats(i);
                printEnterpriseIDStats(i);
                printStartTimeStats(i);
                printClientTimeStats(i);
                printIntermediaryTimeStats(i);
                printEnterpriseTimeStats(i);
                printToatalTimeStats(i);
                fprintf(statFile,"\n");
            }
        }
    }
    fclose(statFile);
}

int compareStats(char a, char b) {
    if (a == b)
        return 0;
    else
        return 1;
}

void printNumStats() {
    for (int i = 0; i < dataGlobal->n_clients; i++) {
        if (dataGlobal->client_stats[i] > 0) {
            fprintf(statFile, "Client %d received %d operation(s)!.\n", i, dataGlobal->client_stats[i]);
        }
    }
    for (int i = 0; i < dataGlobal->n_intermediaries; i++) {
        if (dataGlobal->intermediary_stats[i] > 0) {
            fprintf(statFile, "Intermediary %d prepared %d operation(s)!.\n", i, dataGlobal->intermediary_stats[i]);
        }
    }
    for (int i = 0; i < dataGlobal->n_enterprises; i++) {
        if (dataGlobal->enterprise_stats[i] > 0) {
            fprintf(statFile, "Enterprise %d executed %d operation(s)!.\n", i, dataGlobal->enterprise_stats[i]);
        }
    }
}

void printClientIDStats(int i) {
    struct timespec tsC = dataGlobal->results[i].client_time;
    fprintf(statFile,"Client id: ");
    fprintf(statFile,"%d", dataGlobal->results[i].requesting_client);
    fprintf(statFile, "\n");
}

void printEnterpriseIDStats(int i) {
    struct timespec tsC = dataGlobal->results[i].client_time;
    fprintf(statFile,"Enterprise id: ");
    fprintf(statFile,"%d", dataGlobal->results[i].requested_enterp);
    fprintf(statFile,"\n");
}

void printIntermediaryIDStats(int i) {
    struct timespec tsC = dataGlobal->results[i].client_time;
    fprintf(statFile,"Intermediary id: ");
    fprintf(statFile,"%d", dataGlobal->results[i].receiving_interm);
    fprintf(statFile,"\n");
}

void printStartTimeStats(int i) {
    struct timespec ts = dataGlobal->results[i].start_time;
    time_t t = ts.tv_sec;
    struct tm *local_time = localtime(&t);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(statFile,"Start time: ");
    fprintf(statFile,timeStr);
    fprintf(statFile,"\n");
}

void printClientTimeStats(int i) {
    struct timespec tsC = dataGlobal->results[i].client_time;
    time_t t = tsC.tv_sec;
    struct tm *local_time = localtime(&t);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(statFile,"Client time: ");
    fprintf(statFile,timeStr);
    fprintf(statFile,"\n");
}

void printIntermediaryTimeStats(int i) {
    struct timespec tsI = dataGlobal->results[i].intermed_time;
    time_t t = tsI.tv_sec;
    struct tm *local_time = localtime(&t);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(statFile,"Intermediary time: ");
    fprintf(statFile,timeStr);
    fprintf(statFile,"\n");
}

void printEnterpriseTimeStats(int i) {
    struct timespec tsE = dataGlobal->results[i].enterp_time;
    time_t t = tsE.tv_sec;
    struct tm *local_time = localtime(&t);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(statFile,"Enterprise time: ");
    fprintf(statFile,timeStr);
    fprintf(statFile,"\n");
}

void printToatalTimeStats(int i) {
    struct timespec ts = dataGlobal->results[i].start_time;
    struct timespec tsE = dataGlobal->results[i].enterp_time;
    time_t tS = ts.tv_sec;
    time_t tE = tsE.tv_sec;
    double diff_t = difftime(tE, tS);
    fprintf(statFile,"Total time: ");
    fprintf(statFile,"%f\n", diff_t);
}