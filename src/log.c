#include "log.h"
#include "aptime.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void writeLog(char* filename, char* toWrite, struct timespec* time) {
    FILE *txtfile = fopen(filename, "w");
    if (txtfile == NULL){
        perror("Ficheiro de Log.");
        exit(1);
    }
    time_t t = time->tv_sec;
    struct tm *local_time = localtime(&t);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", local_time);
    fseek(txtfile, 0, SEEK_END);
    fprintf(txtfile, "%s", timeStr);
    fprintf(txtfile, " %s\n", toWrite);
    fclose(txtfile);
}