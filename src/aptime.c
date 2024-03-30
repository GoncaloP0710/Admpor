#include "aptime.h"
#include <time.h>

// Function to get current time as timespec struct
struct timespec get_current_time() {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time;
}

/*
         // Convert timespec to time_t struct
        time_t t = time.tv_sec;
        struct tm *local_time = localtime(&t);

        // Print current time
        printf(asctime(local_time));
        printf("\n");
*/