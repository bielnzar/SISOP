#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int server_fd;

void connect_to_server() {
    struct sockaddr_in serv_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }
}

void handle_command(const char *command, char *username, char *channel, char *room) {
    if (command == NULL) {
        printf("Perintah tidak boleh kosong\n");
        return;
    }

    if (send(server_fd, command, strlen(command), 0) < 0) {
        perror("Gagal mengirim perintah ke server");
    }

    char response[BUFFER_SIZE];
    memset(response, 0, sizeof(response));
    int n = 0;
    if (recv(server_fd, response, BUFFER_SIZE - 1, 0) < 0) {
        perror("Cannot Receive");
    } else {
        if (strstr(response, "Key:") != NULL) {
            char key[50];
            printf("Key: ");
            fgets(key, sizeof(key), stdin);
            key[strcspn(key, "\n")] = '\0'; // Remove newline character

            if (send(server_fd, key, strlen(key), 0) < 0) {
                perror("Gagal mengirim key ke server");
            }

            // Receive the response after sending the key
            memset(response, 0, sizeof(response));
            if (recv(server_fd, response, BUFFER_SIZE - 1, 0) < 0) {
                perror("Cannot Receive");
            } else {
                if (strstr(response, "Key salah") != NULL) {
                    // Reset state if key is invalid
                    if (strlen(room) > 0) {
                        room[0] = '\0';
                    } else if (strlen(channel) > 0) {
                        channel[0] = '\0';
                    }
                }
            }
        } else if (strstr(response, "tidak ada") != NULL || strstr(response, "Key salah") != NULL || strstr(response, "Anda telah diban") != NULL) {
            if (strlen(room) > 0) {
                room[0] = '\0';
            } else if (strlen(channel) > 0) {
                channel[0] = '\0';
            }
            printf("%s\n", response);
        } else if (strstr(response, "Anda telah keluar dari aplikasi") != NULL) {
            close(server_fd);
            exit(0);  // Exit client program after receiving exit confirmation
        } else if(strncmp(command, "JOIN ", 5) == 0){
            n++;
        } else {
            printf("%s\n", response);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Penggunaan: %s REGISTER/LOGIN username -p password\n", argv[0]);
        return 1;
    }

    connect_to_server();

    char command[BUFFER_SIZE];
    memset(command, 0, sizeof(command));

    char username[50];
    char channel[50] = "";
    char room[50] = "";

    if (strcmp(argv[1], "REGISTER") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s REGISTER username -p password\n", argv[0]);
            return 1;
        }

        snprintf(username, sizeof(username), "%s", argv[2]);
        char *password = argv[4];

        snprintf(command, sizeof(command), "REGISTER %s -p %s", username, password);
    } else if (strcmp(argv[1], "LOGIN") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s LOGIN username -p password\n", argv[0]);
            return 1;
        }

        snprintf(username, sizeof(username), "%s", argv[2]);
        char *password = argv[4];

        snprintf(command, sizeof(command), "LOGIN %s -p %s", username, password);

        if (send(server_fd, command, strlen(command), 0) < 0) {
            perror("Gagal mengirim perintah ke server");
        }

        char response[BUFFER_SIZE];
        memset(response, 0, sizeof(response));

        if (recv(server_fd, response, BUFFER_SIZE - 1, 0) < 0) {
            perror("Cannot Receive");
        } else {
            printf("%s\n", response);

            if (strstr(response, "berhasil login") != NULL) {
                while (1) {
                    if (strlen(room) > 0) {
                        printf("[%s/%s/%s] ", username, channel, room);
                    } else if (strlen(channel) > 0) {
                        printf("[%s/%s] ", username, channel);
                    } else {
                        printf("[%s] ", username);
                    }

                    if (fgets(command, BUFFER_SIZE, stdin) == NULL) {
                        printf("Gagal membaca perintah\n");
                        continue;
                    }
                    command[strcspn(command, "\n")] = '\0';

                    if (strncmp(command, "JOIN ", 5) == 0) {
                        if (strlen(channel) == 0) {
                            snprintf(channel, sizeof(channel), "%s", command + 5);
                        } else {
                            snprintf(room, sizeof(room), "%s", command + 5);
                        }
                    } else if (strcmp(command, "EXIT") == 0) {
                        if (strlen(room) > 0) {
                            room[0] = '\0';
                        } else if (strlen(channel) > 0) {
                            channel[0] = '\0';
                        }
                    }

                    handle_command(command, username, channel, room);
                }
            }
        }

        close(server_fd);
        return 0;
    } else {
        printf("Perintah tidak valid\n");
        return 1;
    }

    handle_command(command, username, channel, room);

    close(server_fd);
    return 0;
}