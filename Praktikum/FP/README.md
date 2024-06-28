# Sisop-FP-2024-MH-IT30

Sistem Operasi Sederhana DiscorIT

## [FUN PROJECT](https://its.id/m/SisopFunProject)

Mata Kuliah Sistem Operasi

Dosen pengampu : Ir. Muchammad Husni, M.Kom.

## Kelompok Praktikum [IT-30]

- [Fico Simhanandi - 50272310](https://github.com/PuroFuro)
- [Jody Hezekiah - 5027221050](https://github.com/imnotjs)
- [Nabiel Nizar Anwari - 5027231087](https://github.com/bielnzar)

### Note

Agar penjelasan tidak terlalu panjang, maka kode akan dijelaskan inti-intinya atau secara singkat cara kerjanya saja dikarenakan kode berupa ribuan lines.

## Discorit.c

```c
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
```

Yang dilakukan oleh `discorit.c` adalah ia menjadi client yang akan menerima input dari user, dia akan membagi-bagi input tersebut menggunakan `argv[]` yang dimana nanti perintah tersebut akan dicek dan akan dikirim ke server. Ia diletakan dalam while loop yang berjalan selamanya agar dapat selalu melakukan umpan balik message.

Ketika login, maka code ini akan menyesuaikan tulisan [nama][nama_channel][nama_room] sesuai dengan login dan kosong tidaknya variabel channel/room. Dan semua command akan dikirim ke function `handle_command` yang alhasil akan dikirim ke server.

## Server.c (sebagian)

```c
typedef struct {
    int socket;
    struct sockaddr_in address;
    char logged_in_user[50];
    char logged_in_role[10];
    char logged_in_channel[50];
    char logged_in_room[50];
} client_info;

client_info *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
void *handle_client(void *arg);
void register_user(int client_fd, char *username, char *password);
void login(int client_fd, char *username, char *password);
bool channel_exists(const char *channel_name);
void list_channels(int client_fd, char *username);
void join_channel(int client_fd, char *username, char *channel_name, char *key);
bool check_channel_key(const char *channel_name, const char *key);
void list_rooms(int client_fd, char *username, char *channel_name);
void list_users(int client_fd, char *username);
void chat(int client_fd, char *username, char *channel_name, char *room_name, char *message);
void see_chat(int client_fd, char *username, char *channel_name, char *room_name);
void edit_chat(int client_fd, char *username, char *channel_name, char *room_name, int message_id, char *new_message);
void del_chat(int client_fd, char *username, char *channel_name, char *room_name, int message_id);
void create_channel(int client_fd, char *username, char *channel_name, char *key);
void edit_channel(int client_fd, char *username, char *old_channel_name, char *new_channel_name);
void del_channel(int client_fd, char *username, char *channel_name);
void create_room(int client_fd, char *username, char *channel_name, char *room_name);
void join_room(int client_fd, char *username, char *room_name);
void edit_room(int client_fd, char *username, char *channel_name, char *old_room_name, char *new_room_name);
void del_room(int client_fd, char *username, char *channel_name, char *room_name);
void del_all_rooms(int client_fd, char *username, char *channel_name);
void ban_user(int client_fd, char *username, char *channel_name, char *user_to_ban);
void unban_user(int client_fd, char *username, char *channel_name, char *user_to_unban);
void remove_user(int client_fd, char *username, char *channel_name, char *user_to_remove);
void edit_profile(int client_fd, char *username, char *new_username, char *new_password);
void exit_discorit(int client_fd, char *username, char *channel_name, char *room_name);
void log_activity(char *username, char *action, char *description);
void create_directory(const char *path);
void create_file(const char *path);
```

Agar readme tidak terlalu panjang, maka akan dijelaskan function-function secara singkat beserta fotonya saja.

### 1. void *handle_client(void *arg);

Function ini yang menerima segala command yang diberikan oleh client side seperti misal JOIN, EDIT, CREATE, dan lain sebagainya.

### 2. void register_user(int client_fd, char *username, char *password);

Fucntion ini meregister user dan meletakannya ke dalam users.csv sehingga dapat diakses oleh client saat mereka ingin login.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/2.png)

### 3. void login(int client_fd, char *username, char *password);

Mirip halnya dengan register, ia memberikan akses kepada user yang sudah register untuk anggapannya masuk ke server DiscorIT dan merubah tempat input sehingga memiliki `[nama]`.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/3.png)

### 4. bool channel_exists(const char *channel_name);

Ini hanyalah fungsi pembantu yang dapat mengecek apabila channel sudah ada atau tidak menggunakan bantuan strtok

### 5. void create_channel(int client_fd, char *username, char *channel_name, char *key);

User dapat membuat sebuah channel yang nantinya akan memodifikasi directory menjadi 
```
DiscorIT/
- channels.csv
- users.csv
- channel1/
    - admin/
        - auth.csv
        - user.log
```

Dan channels.csv akan terbuat dengan isinya adalah nama channel yang dibuat tadi

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/5.png)

### 6. void list_channels(int client_fd, char *username);

User dapat mengecek isi dari channel yang ada didalam server dan apabila tidak ada maka outputnya berupa tidak ada channel. Fungsi ini mengambil data yang ada di channels.csv dan dipotong menggunakan strtok agar yang diambil hanyalah nama dari channelnya saja.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/6.png)

### 7. void edit_channel(int client_fd, char *username, char *old_channel_name, char *new_channel_name); [Bagian Revisi]

Simpel saja, edit_channel disini berfungsi untuk mengganti nama dari channel yang sudah ada

**Before**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/5.png)

**After**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/7_5.png)

### 8. void del_channel(int client_fd, char *username, char *channel_name); [Bagian Revisi]

Mirip dengan edit_channel tetapi bedanya ia menghapus channel tersebut

**Before**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/7_5.png)

**After**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/8_7.png)

### 9. void join_channel(int client_fd, char *username, char *channel_name, char *key);

Apabila terdapat channel, maka user dapat join ke channel tersebut dan memasukan `key` yang sudah diset untuk channel tersebut.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/9.png)

### 10. bool check_channel_key(const char *channel_name, const char *key);

Ini hanyalah function pembantu

### 11. void create_room(int client_fd, char *username, char *channel_name, char *room_name);

Sama kerjanya dengan create_channel hanya bedanya ini untuk room

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/11.png)

### 12. void join_room(int client_fd, char *username, char *room_name);

Sama kerjanya dengan join_channel hanya bedanya ini untuk room

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/12.png)

### 13. void edit_room(int client_fd, char *username, char *channel_name, char *old_room_name, char *new_room_name); [Bagian Revisi]

Sama kerjanya seperti edit_channel hanya bedanya ini untuk room

**Before**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/11.png)

**After**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/13_11.png)

### 14. void del_room(int client_fd, char *username, char *channel_name, char *room_name); [Bagian Revisi]

Sama kerjanya seperti del_channel hanya bedanya ini untuk room

**Before**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/11.png)

**After**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/14_11.png)

### 15. void list_users(int client_fd, char *username); [Bagian Revisi]

Fungsi ini membaca nama client yang berada dalam file auth.csv yang ada di channel

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/15.png)

### 16. void exit_discorit(int client_fd, char *username, char *channel_name, char *room_name); [Bagian Revisi]

Fungsi ini mengeluarkan client dari room/channel atau dari server itu sendiri

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/18.png)

### 17. void log_activity(char *username, char *action, char *description);

Fungsi ini sebenarnya tidak perlu tapi dibuat sebagai debugging

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/fp/19.png)

### 18. void chat(int client_fd, char *username, char *channel_name, char *room_name, char *message);

Fungsi ini membaca client berada di room mana dan meng-printkan apa yang ditulis client tersebut di monitor dan juga dimasukan ke dalam chat.csv sesuai dengan masing-masing room

![github-small](https://github.com/bielnzar/sisop/blob/main/FP/chat.png)

### 19. void see_chat(int client_fd, char *username, char *channel_name, char *room_name)

Fungsi ini membaca apa yang telah ditulis di chat.csv yang berada di room tersebut, agar user bisa melihat chat yang telah dituliskan

![github-small](https://github.com/bielnzar/sisop/blob/main/FP/see_chat.png)

## 20. void edit_chat(int client_fd, char *username, char *channel_name, char *room_name, int message_id, char *new_message); [Bagian Revisi]

Fungsi baru ini bisa untuk mengedit Chat yang sebelumnya sudah ada :

![github-small](https://github.com/bielnzar/sisop/blob/main/FP/edit_chat.png)

## 21. void del_chat(int client_fd, char *username, char *channel_name, char *room_name, int message_id) [Bagian Revisi]

Fungsi ini berguna untuk menghapus chat yang sudah ada sebelumnya

![github-small](https://github.com/bielnzar/sisop/blob/main/FP/del-chat.png)

## Monitor.c

Untuk bagian monitor.c yang ia lakukan adalah membaca file .csv secara terus menerus sehingga bisa dibilang ia membacanya secara real time

```c
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
```

**Gambar Monitor dijalankan**

![github-small](https://github.com/bielnzar/sisop/blob/main/FP/monitor.png)

## Pada Bagian yang diberi keterangan [Bagian Revisi] merupakan fungsi yang perlu revisi setelah demo dengan asisten

### ~ Sekian Laporan Resmi dari kami kelompok IT30, Semoga nilai Akumulasi praktikum kami maksimal, terimakasih kami haturkan kepada seluruh Asisten yang telah membersamai kami dalam mempelajari mata kuliah Sistem Operasi ini, Kami juga memohonkan maaf apabila banyak terdapat kesalahan dan kekurangan, sekian dari kami IT30, TERIMAKASIH :>
