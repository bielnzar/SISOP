#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <crypt.h>
#include <sys/select.h>
#include <stdbool.h>

#define MAX_LEN 256
#define USERS_FILE "DiscorIT/users.csv"

int authenticate(const char *username, const char *password) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        perror("Failed to open USERS_FILE");
        return 0;
    }

    char line[256];
    bool user_found = false;
    char stored_password[128];

    while (fgets(line, sizeof(line), file)) {
        char *stored_id = strtok(line, ",");
        char *stored_username = strtok(NULL, ",");
        char *stored_hashed_password = strtok(NULL, ",");
        char *stored_role = strtok(NULL, ",");

        if (stored_hashed_password) {
            stored_hashed_password[strcspn(stored_hashed_password, "\n")] = '\0';
        }

        if (stored_username && strcmp(stored_username, username) == 0) {
            user_found = true;
            strcpy(stored_password, stored_hashed_password);
            break;
        }
    }
    fclose(file);

    if (!user_found) {
        printf("Username tidak terdaftar\n");
        return 0;
    } else {
        char input_hashed_password[128];
        strcpy(input_hashed_password, crypt(password, stored_password));
        if (strcmp(stored_password, input_hashed_password) != 0) {
            printf("Password salah\n");
            return 0;
        } else {
            printf("username berhasil login\n");
            return 1;
        }
    }
}

void monitor_chat(const char *username, const char *channel, const char *room) {
    char filepath[MAX_LEN];
    char filepath_monitor[MAX_LEN];
    snprintf(filepath, sizeof(filepath), "DiscorIT/%s/%s/chat.csv", channel, room);
    snprintf(filepath_monitor, sizeof(filepath_monitor), "DiscorIT/%s/chat.csv", channel);

    FILE *file = fopen(filepath_monitor, "r");
    if (!file) {
        perror("Error opening chat file");
        return;
    }

    printf("[%s] -channel %s -room %s\n\n", username, channel, room);

    char line[MAX_LEN];
    long last_position = 0;

    while (1) {
        int new_messages = 0;
        fseek(file, last_position, SEEK_SET);
        while (fgets(line, sizeof(line), file)) {
            char date[20], time[20], id[10], chat_username[MAX_LEN], text[MAX_LEN];
            if (sscanf(line, "%s %s %s %s %[^\n]", date, time, id, chat_username, text) == 5) {
                printf("[%s %s][%s][%s] \"%s\"\n", date, time, id, chat_username, text);
                new_messages = 1;
            }
            last_position = ftell(file);
        }

        if (new_messages) {
            fflush(stdout);
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

        if (ret > 0) {
            char command[MAX_LEN];
            if (fgets(command, sizeof(command), stdin)) {
                if (strncmp(command, "EXIT", 4) == 0) {
                    printf("\n[%s/%s/%s] EXIT\n", username, channel, room);
                    printf("[%s] EXIT\n", username);
                    break;
                }
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "LOGIN") != 0 || strcmp(argv[3], "-p") != 0) {
        fprintf(stderr, "Usage: %s LOGIN <username> -p <password>\n", argv[0]);
        return 1;
    }

    char *username = argv[2];
    char *password = argv[4];

    if (!authenticate(username, password)) {
        return 1;
    }

    char channel[MAX_LEN], room[MAX_LEN];
    char input[MAX_LEN];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }
    input[strcspn(input, "\n")] = 0;

    if (sscanf(input, "-channel %s -room %s", channel, room) != 2) {
        fprintf(stderr, "Invalid format. Yang Benar : -channel <channel_name> -room <room_name>\n");
        return 1;
    }

    monitor_chat(username, channel, room);

    return 0;
}