#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAXCHAR 1000
#define NEWDATADIR "/home/purofuro/Fico/M3S1/newdata/"
#define TRASH "Trash Can"
#define PARK "Parking Lot"

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(char) * 1024, IPC_CREAT | 0666);
    char *csvdata = shmat(shmid, NULL, 0);

    while (*csvdata != '\0') {
        char datadir[MAXCHAR]; // Initialize datadir
        char filename[100];
        strcpy(datadir, NEWDATADIR); // Set the base directory
        strcpy(filename, csvdata);
        strcat(datadir, csvdata); // Concatenate the filename

        csvdata += strlen(csvdata) + 1;

        FILE *open = fopen(datadir, "r");
        if (open == NULL) {
            printf("Error opening file: %s\n", datadir);
            return 1;
        }

        char row[MAXCHAR];
        char *token, best[100], type[20]; // Initialize best and type
        int skip = 0;
        float max = 0.0;

        if (strstr(datadir, "trashcan") != NULL) strcpy(type, TRASH);
        else if(strstr(datadir, "parkinglot") != NULL) strcpy(type, PARK);

        while (fgets(row, MAXCHAR, open) != NULL) {
            if (skip == 0) {
                skip++;
                continue;
            }
            token = strtok(row, ",");
            while (token != NULL) {
                char tempname[100];
                if (isdigit(token[0]) || isdigit(token[1])) {
                    float temp = atof(token);
                    if (temp >= max) {
                        max = temp;
                        strcpy(best, tempname);
                    }
                } else {
                    strcpy(tempname, token);
                }
                token = strtok(NULL, ",");
            }
        }
        printf("Type: %s\nFilename: %s\n-----------------------------------\nName: %s\nRating: %0.1f\n\n",type, filename, best, max);

        fclose(open); // Close the file
    }

    shmdt(csvdata);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
