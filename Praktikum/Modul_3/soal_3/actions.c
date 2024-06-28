#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

char *GapFunc(char *gappy){
    char temp[10], temp1[10];
    sscanf(gappy, "%s %s", temp, temp1);
    float jarak = atof(temp1);
    if(jarak <= 3.5) return "Gogogo";
    else if(jarak > 3.5 && jarak <= 10) return "Push";
    else if(jarak > 10) return "Stay out of trouble";
}

char *FuelFunc(char *gingseng){
    char temp[10], temp1[10];
    sscanf(gingseng, "%s %s", temp, temp1);
    float bengsing = atof(temp1);
    if(bengsing > 80) return "Push Push Push";
    else if(bengsing >= 50 && bengsing <= 80) return "You can go";
    else if(bengsing < 50) return "Conserve Fuel";
}

char *TireFunc(char *tired){
    char temp[10], temp1[10];
    sscanf(tired, "%s %s", temp, temp1);
    int capek = atoi(temp1);
    if(capek > 80) return "Go Push Go Push";
    else if(capek > 50 && capek <= 80) return "Good Tire Wear";
    else if(capek >= 30 && capek <= 50) return "Conserve Your Tire";
    else if(capek < 30) return "Box Box Box";
}

char *ChangeFunc(char *SebutkanTipePasanganKamu){
    if(strstr(SebutkanTipePasanganKamu, "Soft") != NULL) return "Mediums Ready";
    else if(strstr(SebutkanTipePasanganKamu, "Medium") != NULL) return" Box for Soft";
}

char *NoneOfYourInputIsCorrectYouKnow(){
    return "Kesalahan Berpikir";
}   

void Printlog(char *source, char *action, char *afteraction) {
    char log[2048] = "/home/purofuro/Fico/M3S3/race.log";
    FILE *race = fopen(log, "a");
    if (race == NULL) {
        printf("Error opening log file\n");
        return;
    }
    time_t my_time;
    struct tm * timeinfo; 
    time (&my_time);
    timeinfo = localtime (&my_time);
    fprintf(race, "[%s] [%02d/%02d/%04d %02d:%02d:%02d]: [%s] [%s]\n", source,timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,action, afteraction);
    fclose(race);
}

