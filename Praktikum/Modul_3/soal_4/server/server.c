#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAXLINE 1024
#define MAX_ANIME 100

struct Anime
{
    char hari[20];
    char genre[20];
    char judul[50];
    char status[20];
};

char *get_current_time()
{
    time_t now;
    time(&now);
    return ctime(&now);
}

void write_to_log(const char *type, const char *message)
{
    FILE *fp;
    time_t now;
    struct tm *local;
    char timestamp[20];

    time(&now);
    local = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%y", local);

    fp = fopen("change.log", "a");
    if (fp == NULL)
    {
        printf("Error opening log file.\n");
        return;
    }

    fprintf(fp, "[%s] [%s] %s\n", timestamp, type, message);
    fclose(fp);
}

void read_myanimelist(struct Anime myanimelist[], int *count)
{
    FILE *fp;
    char line[MAXLINE];
    *count = 0;

    fp = fopen("myanimelist.csv", "r");
    if (fp == NULL)
    {
        printf("File not found!\n");
        return;
    }

    while (fgets(line, MAXLINE, fp) != NULL)
    {
        sscanf(line, "%[^,],%[^,],%[^,],%s", myanimelist[*count].hari, myanimelist[*count].genre, myanimelist[*count].judul, myanimelist[*count].status);
        (*count)++;
    }

    fclose(fp);
}

void send_anime_list(int sockfd, struct Anime myanimelist[], int count)
{
    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));

    for (int i = 0; i < count; i++)
    {
        sprintf(buffer + strlen(buffer), "%d. %s\n", i + 1, myanimelist[i].judul);
    }

    send(sockfd, buffer, strlen(buffer), 0);
}

void send_genre_list(int sockfd, struct Anime myanimelist[], int count, char genre[])
{
    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));
    int found = 0;
    int num = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(myanimelist[i].genre, genre) == 0)
        {
            sprintf(buffer + strlen(buffer), "%d. %s\n", ++num, myanimelist[i].judul);
            found = 1;
        }
    }

    if (!found)
    {
        sprintf(buffer, "Tidak ada anime dengan genre %s.\n", genre);
    }

    send(sockfd, buffer, strlen(buffer), 0);
}

void send_day_list(int sockfd, struct Anime myanimelist[], int count, char day[])
{
    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));
    int found = 0;
    int num = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(myanimelist[i].hari, day) == 0)
        {
            sprintf(buffer + strlen(buffer), "%d. %s\n", ++num, myanimelist[i].judul);
            found = 1;
        }
    }

    if (!found)
    {
        sprintf(buffer, "Tidak ada anime yang tayang di hari %s.\n", day);
    }

    send(sockfd, buffer, strlen(buffer), 0);
}

void send_status(int sockfd, struct Anime myanimelist[], int count, char judul[])
{
    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));
    int found = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(myanimelist[i].judul, judul) == 0)
        {
            sprintf(buffer, "%s\n", myanimelist[i].status);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        sprintf(buffer, "Judul %s tidak ditemukan.\n", judul);
    }

    send(sockfd, buffer, strlen(buffer), 0);
}

void add_anime(struct Anime myanimelist[], int *count, char input[])
{
    char hari[20], genre[20], judul[50], status[20];
    sscanf(input, "%[^,],%[^,],%[^,],%s", hari, genre, judul, status);

    FILE *fp;
    fp = fopen("myanimelist.csv", "a");
    if (fp == NULL)
    {
        printf("File not found!\n");
        return;
    }

    fprintf(fp, "%s,%s,%s,%s\n", hari, genre, judul, status);
    fclose(fp);

    strcpy(myanimelist[*count].hari, hari);
    strcpy(myanimelist[*count].genre, genre);
    strcpy(myanimelist[*count].judul, judul);
    strcpy(myanimelist[*count].status, status);
    (*count)++;

    // Log perubahan
    write_to_log("ADD", judul);
}

void edit_anime(struct Anime myanimelist[], int count, char input[])
{
    char judul_lama[50], hari[20], genre[20], judul_baru[50], status[20];
    sscanf(input, "%[^,],%[^,],%[^,],%[^,],%s", judul_lama, hari, genre, judul_baru, status);

    int found = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(myanimelist[i].judul, judul_lama) == 0)
        {
            strcpy(myanimelist[i].hari, hari);
            strcpy(myanimelist[i].genre, genre);
            strcpy(myanimelist[i].judul, judul_baru);
            strcpy(myanimelist[i].status, status);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Judul %s tidak ditemukan.\n", judul_lama);
    }
    else
    {
        FILE *fp;
        fp = fopen("myanimelist.csv", "w");
        if (fp == NULL)
        {
            printf("File not found!\n");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            fprintf(fp, "%s,%s,%s,%s\n", myanimelist[i].hari, myanimelist[i].genre, myanimelist[i].judul, myanimelist[i].status);
        }

        fclose(fp);

        // Log perubahan
        write_to_log("EDIT", judul_lama);
    }
}

void delete_anime(struct Anime myanimelist[], int *count, char judul[])
{
    int found = 0;

    for (int i = 0; i < *count; i++)
    {
        if (strcmp(myanimelist[i].judul, judul) == 0)
        {
            for (int j = i; j < *count - 1; j++)
            {
                myanimelist[j] = myanimelist[j + 1];
            }
            (*count)--;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Judul %s tidak ditemukan.\n", judul);
    }

    FILE *fp;
    fp = fopen("myanimelist.csv", "w");
    if (fp == NULL)
    {
        printf("File not found!\n");
        return;
    }

    for (int i = 0; i < *count; i++)
    {
        fprintf(fp, "%s,%s,%s,%s\n", myanimelist[i].hari, myanimelist[i].genre, myanimelist[i].judul, myanimelist[i].status);
    }

    fclose(fp);

    // Log perubahan
    write_to_log("DEL", judul);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAXLINE] = {0};

    struct Anime myanimelist[MAX_ANIME];
    int count;

    read_myanimelist(myanimelist, &count);

    // Membuat socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Mengatur opsi socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Mengikat socket ke port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Mendengarkan koneksi
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Menerima koneksi baru
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Menerima dan memproses perintah dari client
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(new_socket, buffer, sizeof(buffer));

        if (valread < 0)
        {
            perror("Error reading from socket");
            continue; // Tetap lanjut ke iterasi berikutnya jika terjadi kesalahan baca
        }

        printf("Received : %s\n\n", buffer);

        if (strcmp(buffer, "exit") == 0)
        {
            printf("Received : exit\n\n");
            break;
        }

        char command[10], temp[100];
        sscanf(buffer, "%s %[^\n]", command, temp);

        if (strcmp(command, "tampilkan") == 0)
        {
            send_anime_list(new_socket, myanimelist, count);
        }
        else if (strcmp(command, "genre") == 0)
        {
            send_genre_list(new_socket, myanimelist, count, temp);
        }
        else if (strcmp(command, "hari") == 0)
        {
            send_day_list(new_socket, myanimelist, count, temp);
        }
        else if (strcmp(command, "status") == 0)
        {
            send_status(new_socket, myanimelist, count, temp);
        }
        else if (strcmp(command, "add") == 0)
        {
            char input[MAXLINE];
            memset(input, 0, sizeof(input));
            strcpy(input, temp);
            add_anime(myanimelist, &count, input);
            send(new_socket, "Anime berhasil ditambahkan.\n", strlen("Anime berhasil ditambahkan.\n"), 0);
        }
        else if (strcmp(command, "edit") == 0)
        {
            char input[MAXLINE];
            memset(input, 0, sizeof(input));
            strcpy(input, temp);
            edit_anime(myanimelist, count, input);
            send(new_socket, "Anime berhasil diubah.\n", strlen("Anime berhasil diubah.\n"), 0);
        }
        else if (strcmp(command, "delete") == 0)
        {
            delete_anime(myanimelist, &count, temp);
            send(new_socket, "Anime berhasil dihapus.\n", strlen("Anime berhasil dihapus.\n"), 0);
        }
        else
        {
            send(new_socket, "Invalid Command\n", strlen("Invalid Command\n"), 0);
        }
    }

    close(new_socket);
}