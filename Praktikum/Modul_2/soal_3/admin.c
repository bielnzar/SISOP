#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void start_daemon(const char *username);
void stop_daemon(const char *username);
void displayUserActivity(const char *username);
void logActivity(const char *username);

int main(int argc, char *argv[]) {
    if (argc != 3 || (strcmp(argv[1], "-m") != 0 && strcmp(argv[1], "-s") != 0)) {
        fprintf(stderr, "Usage: %s <-m/-s> <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-m") == 0) {
        start_daemon(argv[2]);
    } else if (strcmp(argv[1], "-s") == 0) {
        stop_daemon(argv[2]);
    } 

    exit(EXIT_SUCCESS);
}

void start_daemon(const char *username) {
    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Daemon started for user %s with PID: %d\n", username, pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    if (chdir("/") < 0) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Daemon code goes here
    while (1) {
        logActivity(username); // Log user activity
        sleep(300); // Check every 5 minutes
    }
}

void stop_daemon(const char *username) {
    pid_t pid;
    FILE *fp;
    char command[50];

    // Find the PID of the process with the given name
    snprintf(command, sizeof(command), "pgrep -u %s admin", username);
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing pgrep");
        exit(EXIT_FAILURE);
    }
    if (fscanf(fp, "%d", &pid) != 1) {
        printf("Process with name %s not found\n", username);
        pclose(fp);
        exit(EXIT_SUCCESS); // No need to stop if the process is not running
    }
    pclose(fp);

    // Send SIGTERM to the process
    if (kill(pid, SIGTERM) == -1) {
        perror("Error killing process");
        exit(EXIT_FAILURE);
    }
    printf("Process with name %s and PID %d killed\n", username, pid);
}

void displayUserActivity(const char *username) {
    char command[100];
    FILE *fp;
    char line[1000];

    // Membuat command shell untuk mencari proses user dengan nama username
    sprintf(command, "ps -u %s", username);

    // Membuka proses shell dan membaca outputnya
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error: Failed to run command\n");
        return;
    }

    // Menampilkan header
    printf("User Activity for: %s\n", username);
    printf("----------------------\n");

    // Membaca setiap baris output dari command shell
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Menampilkan baris output
        printf("%s", line);
    }

    // Menutup proses shell
    pclose(fp);
}

void logActivity(const char *username) {
    FILE *logFile;
    char filename[100]; // Increased buffer size for filename
    char command[1000];
    char line[1000];
    char status[10];
    time_t rawtime;
    struct tm *timeinfo;

    // Get current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format the time
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "[%d:%m:%Y]-[%H:%M:%S]", timeinfo);

    // Specify the absolute path for the log file
    // Change "/path/to/your/folder/" to the actual path
    snprintf(filename, sizeof(filename), "/path/to/your/folder/%s_activity.log", username);

    // Check if log file exists, create new file if it doesn't exist
    if ((logFile = fopen(filename, "a")) == NULL) {
        logFile = fopen(filename, "w");
        if (logFile == NULL) {
            perror("Error creating log file");
            return;
        }
        // Write header to the new log file
        fprintf(logFile, "User Activity Log for: %s\n", username);
        fprintf(logFile, "----------------------\n");
    } else {
        fclose(logFile);
        // Open the log file in append mode
        logFile = fopen(filename, "a");
        if (logFile == NULL) {
            perror("Error opening log file");
            return;
        }
    }

    // Construct command to get user activity
    snprintf(command, sizeof(command), "pgrep -u %s admin", username);

    // Open command as a process
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        fclose(logFile);
        return;
    }

    // Check if the process is running
    if (fgets(line, sizeof(line), fp) != NULL) {
        strcpy(status, "JALAN");
    } else {
        strcpy(status, "GAGAL");
    }

    // Write the activity to the log file
    fprintf(logFile, "%s-pid_kegiatan-nama_kegiatan_%s\n", timeStr, status);

    // Close the command process and log file
    pclose(fp);
    fclose(logFile);
}
