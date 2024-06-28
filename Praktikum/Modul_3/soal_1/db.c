#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <time.h>
#include <unistd.h>

#define NEWDATADIR "/home/purofuro/Fico/M3S1/newdata/"
#define DATABASEDIR "/home/purofuro/Fico/M3S1/microservices/database/"

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(char) * 1024, IPC_CREAT | 0666);
    char* csvdata = shmat(shmid, NULL, 0);
    char type[20]; // Initialize type
    time_t my_time;
    struct tm* timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);
    FILE* log = fopen("/home/purofuro/Fico/M3S1/microservices/database/db.log", "a");

    while (*csvdata != '\0') {
        char filename[100];
        strcpy(filename, csvdata);
        csvdata += strlen(csvdata) + 1;
        if (strstr(filename, "trashcan") != NULL) strcpy(type, "Trash Can");
        else strcpy(type, "Parking Lot");

        char NDname[100] = NEWDATADIR; // Initialize NDname
        char DDname[100] = DATABASEDIR; // Initialize DDname
        strcat(NDname, filename);
        strcat(DDname, filename);

        if (rename(NDname, DDname) != 0) {
            perror("Error renaming file");
            return 1;
        }

        fprintf(log, "[%d/%d/%d %d:%d:%d] [%s] [%s]\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, type, filename);
    }

    shmdt(csvdata);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(log); // Close the log file
    return 0;
}
