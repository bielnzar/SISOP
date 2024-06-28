#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#define NEWDATADIR "/home/purofuro/Fico/M3S1/newdata/"

int main(){
    key_t key = 1234;
    int shmid = shmget(key, sizeof(char)* 1024, IPC_CREAT | 0666);
    char *csvdata = shmat(shmid, NULL, 0);

    DIR *dr;
    struct dirent *newdata;
    dr = opendir("/home/purofuro/Fico/M3S1/newdata/."); 
    if (dr) {
        while ((newdata = readdir(dr)) != NULL) {
            if((strstr(newdata->d_name, "trashcan.csv") != NULL || strstr(newdata->d_name, "parkinglot.csv") != NULL)){
                strcpy(csvdata, newdata->d_name);
                csvdata += strlen(newdata->d_name) + 1; //Go to the next line or array
            }
            else if(strcmp(newdata->d_name, ".") != 0 && strcmp(newdata->d_name, "..") != 0){
                char temp[100] = NEWDATADIR;
                strcat(temp, newdata->d_name);
                remove(temp);
            }
        }
        
    }
    closedir(dr); 
    *csvdata = '\0';
    shmdt(csvdata);
    return 0;

}
