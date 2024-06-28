#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

int default_mode = 1;
int backup_mode = 1;
int restore_mode = 1; 
int exiting = 0; 

void timelog(char *name, char *action){

    char result[1000];
    time_t my_time;
    struct tm * timeinfo; 
    time (&my_time);
    timeinfo = localtime (&my_time);

    if(strstr(action, "restore") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully restored from backup.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);

    } else if(strstr(action, "backup") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully moved to backup.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);

    
    } else if(strstr(action, "delete") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully Deleted.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);

    
    } else if(strstr(action, "rename") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully Renamed.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);

    }
    FILE *file = fopen("/home/purofuro/Fico/Soal_2/history.log", "a"); 
    if (file != NULL) {  
        fprintf(file, "%s\n", result); 
        fclose(file); 
    } else { 
        printf("Error opening the file\n"); 
    } 
}

//Check if file already exists or not
int ada(const char *fname){
    char fixname[100] = "/home/purofuro/Fico/Soal_2/";
    strcat(fixname, fname);
    if (access(fixname, F_OK) == 0) {
        return 1;
    } else {    
        return 0;
    }
}
int rename_stuff(){
    char *basePath = "/home/purofuro/Fico/Soal_2/library";
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return 0;

    while ((dp = readdir(dir)) != NULL)
    {   
        
        if(default_mode == 1){
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){

                char to_change[1000] = "/home/purofuro/Fico/Soal_2/library/";
                char original_name[1000] = "/home/purofuro/Fico/Soal_2/library/";
                char text[100], ch;
                int key = 19;
                strcpy(text, dp->d_name);
                if(text[0] >= '0' && text[0] <= '9'){

                    if(strstr(dp->d_name, "d3Let3") != NULL){
                        strcat(to_change, dp->d_name);
                        remove(to_change);
                        timelog(text, "delete");

                    } else if(strstr(text, "r3N4mE") != NULL){
                        //rename the files
                        strcat(to_change, dp->d_name);

                        if(strstr(text, ".ts") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/helper.ts");
                            timelog("helper.ts", "rename");
                        }
                        else if(strstr(text, ".py") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/calculator.py");
                            timelog("calculator.py", "rename");
                        }
                        else if(strstr(text, ".go") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/server.go");
                            timelog("server.go", "rename");
                        }
                        else{
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/renamed.file");
                            timelog("renamed.file", "rename");
                        }
                    }

                } else{
                    for (int i = 0; text[i] != '\0'; ++i) {

                        ch = text[i];
                        // Check for valid characters.
                        if (isalnum(ch)) {
                            //Lowercase characters.
                            if (islower(ch)) {
                                ch = (ch - 'a' - key + 26) % 26 + 'a';
                            }
                            // Uppercase characters.
                            if (isupper(ch)) {
                                ch = (ch - 'A' - key + 26) % 26 + 'A';
                            }
                            // Numbers.
                        }
                        // Adding decoded character back.
                        text[i] = ch;

                    }
                    if(strstr(text, "d3Let3") != NULL){
                        strcat(to_change, dp->d_name);
                        remove(to_change);
                        timelog(dp->d_name, "delete");

                    } else if(strstr(text, "r3N4mE") != NULL){
                        //rename the files
                        strcat(to_change, dp->d_name);
                        if(strstr(text, ".ts") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/helper.ts");
                            timelog("helper.ts", "rename");
                        }
                        else if(strstr(text, ".py") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/calculator.py");
                            timelog("calculator.py", "rename");
                        }
                        else if(strstr(text, ".go") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/server.go");
                            timelog("server.go", "rename");
                        }
                        else{
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/renamed.file");
                            timelog("renamed.file", "rename");
                        }

                    } else if(strstr(text, "m0V3") != NULL){
                        strcat(original_name, dp->d_name);
                        strcat(to_change, text);
                        rename(original_name, to_change);
                        timelog(dp->d_name, "rename");
                    }
                }  
            }
        }
        sleep(1);
    }
    closedir(dir);
}

int default_act(){

    int status;
    if(ada("library.zip") == 0){
        pid_t download_child = fork();
        if(download_child < 0){
            printf("garpu gagal");
            exit(EXIT_FAILURE);
        }
        else if(download_child == 0){
            backup_mode = 0;
            char *argv[6] = {"Download", "--content-disposition", "https://docs.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "-P", "/home/purofuro/Fico/Soal_2", NULL};
            execv("/bin/wget", argv);
        }
        
    } 
    wait(&status);
    if(WIFEXITED(status)){
        if(ada("history.log") == 0){
            pid_t history_child = fork();
            if(history_child == 0){
                char *argv[3] = {"make a history", "/home/purofuro/Fico/Soal_2/history.log", NULL};
                execv("/bin/touch", argv);
            }
        
        }
    }

    wait(&status);
    if(WIFEXITED(status)){
        if(ada("library") == 0){
            pid_t unzip_child = fork();
            if(unzip_child == 0){
                char *argv[5] = {"unzip", "/home/purofuro/Fico/Soal_2/library.zip", "-d", "/home/purofuro/Fico/Soal_2", NULL};
                execv("/bin/unzip", argv);
            }
            
        } else{
            pid_t lib_delete_child = fork();
            if(lib_delete_child < 0){
                printf("garpu gagal");
                exit(EXIT_FAILURE);
            }

            else if(lib_delete_child == 0){
                backup_mode = 0;
                if(ada("library.zip") != 0){
                    pid_t del_childchild = fork();
                    if(del_childchild == 0){
                        char *argv[4] = {"remove", "-rf", "/home/purofuro/Fico/Soal_2/library", NULL};
                        execv("/bin/rm", argv);
                    }
                    wait(&status);
                    if(WIFEXITED(status)){
                        pid_t zip_child = fork();
                        if(zip_child == 0){
                            char *argv[5] = {"unzip", "/home/purofuro/Fico/Soal_2/library.zip", "-d", "/home/purofuro/Fico/Soal_2", NULL};
                            execv("/bin/unzip", argv);
                        }
                        else{
                            wait(&status);
                            if(WIFEXITED(status)){
                                rename_stuff();
                            }
                        }
                    }
                }
            }
        }
    }
    wait(&status);
    if(WIFEXITED(status)){
        if(ada("library") != 0){
            pid_t mkdir_child = fork();
            if(mkdir_child == 0){
                char *argv[3] = {"making folder", "/home/purofuro/Fico/Soal_2/library/backup", NULL};
                execv("/bin/mkdir", argv);
            }
        }
    }
    wait(&status);
    if(WIFEXITED(status)){
        rename_stuff();
    }
    sleep(3600);
}

int backup_act(){

    int status;
    char *basePath = "/home/purofuro/Fico/Soal_2/library";
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir) return 0;

    while ((dp = readdir(dir)) != NULL){
        
        char filename[1000] = "/home/purofuro/Fico/Soal_2/library/";
        if(backup_mode == 1){
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            
                if(strstr(dp->d_name, "m0V3") != NULL){
                    strcat(filename, dp->d_name);
                    pid_t multi_child = fork();
                    if(multi_child == 0){
                        char *argv[4] = {"backing up", filename, "/home/purofuro/Fico/Soal_2/library/backup", NULL}; 
                        execv("/bin/mv", argv);
                    }
                }
                timelog(dp->d_name, "backup");
                
            }
            sleep(1);
        }
    }
    closedir(dir);
}

int restore_act(){

    int status;
    char *basePath = "/home/purofuro/Fico/Soal_2/library/backup";
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir) return 0;

    while ((dp = readdir(dir)) != NULL){

        
        char filename[1000] = "/home/purofuro/Fico/Soal_2/library/backup/";
        if(restore_mode == 1){
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
                
                if(strstr(dp->d_name, "m0V3") != NULL){
                    strcat(filename, dp->d_name);
                    pid_t multi_child = fork();
                    if(multi_child == 0){
                        char *argv[4] = {"backing up", filename, "/home/purofuro/Fico/Soal_2/library", NULL}; 
                        execv("/bin/mv", argv);
                    }
                }
                timelog(dp->d_name, "restore");
            
            }  
            sleep(1);
        }
    }
    closedir(dir);
}

void signal_mode(int signal){
    if(signal == SIGRTMIN){
        write(STDOUT_FILENO, "Default chosen", 13);
        default_mode = 1;
        restore_mode = 0;
        backup_mode = 0;
       
    } else if(signal == SIGUSR1){
        write(STDOUT_FILENO, "Backup chosen", 13);
        backup_mode = 1;
        default_mode = 0;
        restore_mode = 0;
        backup_act();

    
    } else if(signal == SIGUSR2){
        write(STDOUT_FILENO, "Restore chosen", 13);
        restore_mode = 1;
        backup_mode = 0;
        default_mode = 0;
        restore_act();


    } else if(signal == SIGTERM){
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]){

    signal(SIGRTMIN,signal_mode);
    signal(SIGUSR1, signal_mode);
    signal(SIGUSR2, signal_mode);
    signal(SIGTERM, signal_mode);
    pid_t pid, sid;        // Variabel untuk menyimpan PID

    pid = fork();     // Menyimpan PID dari Child Process

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
    exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
    exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
    exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
    exit(EXIT_FAILURE);
    }
    printf("The PID %d\n", getpid());

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while(1){
        //check
        if(argc == 1){
            if(default_mode == 1) default_act();
        }
        if(argc >= 2){
            if(strcmp(argv[1], "-m") == 0){
                if(backup_mode == 0){
                    if(strcmp(argv[2], "backup") == 0){
                        backup_act();
                    }
                }
                else if(strcmp(argv[2], "restore") == 0){
                    if(restore_mode){
                        restore_act();
                    }
                }
            }
        }
        sleep(1000);
    }
    return 0;
}