#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <crypt.h>
#include <dirent.h>
#include <sys/wait.h>

#define PORT 9001
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define SALT "duwae621h283"
#define USERS_FILE "/home/purofuro/Fico/fpsisop/final/DiscorIT/users.csv"
#define CHANNELS_FILE "/home/purofuro/Fico/fpsisop/final/DiscorIT/channels.csv"
#define AUTH_FILE "/home/purofuro/Fico/fpsisop/final/DiscorIT/auth.csv"
#define CHAT_LOG_FILE "/home/purofuro/Fico/fpsisop/final/DiscorIT/chat.csv"
#define USERS_LOG_FILE "/home/purofuro/Fico/fpsisop/final/DiscorIT/users.log"

int user_count = 0;
int is_in_channel = 0;

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
void remove_function(char *path);
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
int client_id(int client_fd, char *username);

int main() {
    mkdir("/home/purofuro/Fico/fpsisop/final/DiscorIT", 0777);
    create_file(AUTH_FILE);

    int server_fd, client_socket, client_count = 0;
    struct sockaddr_in server_addr, client_addr;
    pthread_t tid;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        
        user_count = 0;

        socklen_t client_len = sizeof(client_addr);

        // Accept connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        client_info *new_client = (client_info *)malloc(sizeof(client_info));
        new_client->socket = client_socket;
        new_client->address = client_addr;
        strcpy(new_client->logged_in_user, "");
        strcpy(new_client->logged_in_role, "");
        strcpy(new_client->logged_in_channel, "");
        strcpy(new_client->logged_in_room, "");

        pthread_mutex_lock(&clients_mutex);
        clients[client_count++] = new_client;
        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&tid, NULL, handle_client, (void *)new_client) != 0) {
            perror("Thread creation failed");
            continue;
        }

        
        if (client_count >= MAX_CLIENTS) {
            printf("Maximum clients reached. No longer accepting connections.\n");
            break;
        }
    }

    
    close(server_fd);

    return 0;
}

char* get_user_role(const char *username) {
    static char role[10];
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        perror("Failed to open USERS_FILE");
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *stored_id = strtok(line, ",");
        char *stored_username = strtok(NULL, ",");
        char *stored_hashed_password = strtok(NULL, ",");
        char *stored_role = strtok(NULL, ",");

        if (stored_role) {
            stored_role[strcspn(stored_role, "\n")] = '\0';
        }

        if (stored_username && strcmp(stored_username, username) == 0) {
            strcpy(role, stored_role);
            fclose(file);
            return role;
        }
    }

    fclose(file);
    return NULL;
}

// Function to handle each client connection
void *handle_client(void *arg) {
    
    client_info *client = (client_info *)arg;
    int client_socket = client->socket;
    char buffer[BUFFER_SIZE] = {0};
    int read_size;

    // Receive message from client
    while ((read_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        char *token;
        char *commands[10];
        int i = 0;

        // Tokenize incoming message
        token = strtok(buffer, " ");
        while (token != NULL && i < 10) {
            commands[i++] = token;
            token = strtok(NULL, " ");
        }
        commands[i] = NULL;
        // Process command based on the first token
        if (strcmp(commands[0], "REGISTER") == 0) {
            if (commands[1] != NULL && strcmp(commands[2], "-p") == 0 && commands[3] != NULL) {
                register_user(client_socket, commands[1], commands[3]);
            } else {
                send(client_socket, "Invalid command format. Usage: REGISTER username -p password\n", strlen("Invalid command format. Usage: REGISTER username -p password\n"), 0);
            }
        } else if (strcmp(commands[0], "LOGIN") == 0) {
            if (commands[1] != NULL && strcmp(commands[2], "-p") == 0 && commands[3] != NULL) {
                login(client_socket, commands[1], commands[3]);
            } else {
                send(client_socket, "Invalid command format. Usage: LOGIN username -p password\n", strlen("Invalid command format. Usage: LOGIN username -p password\n"), 0);
            }
        } else if (strcmp(commands[0], "CREATE") == 0 && strcmp(commands[1], "CHANNEL") == 0) {
            if (commands[2] != NULL && strcmp(commands[3], "-k") == 0 && commands[4] != NULL) {
                create_channel(client_socket, client->logged_in_user, commands[2], commands[4]);
            } else {
                send(client_socket, "Invalid command format. Usage: CREATE CHANNEL channel -k key\n", strlen("Invalid command format. Usage: CREATE CHANNEL channel -k key\n"), 0);
            }
        } else if (strcmp(commands[0], "CREATE") == 0 && strcmp(commands[1], "ROOM") == 0) {
            if (commands[3] != NULL && commands[3] != NULL) {
                create_room(client_socket, client->logged_in_user, client->logged_in_channel, commands[3]);
            } else {
                send(client_socket, "Invalid command format. Usage: CREATE ROOM channel_name room_name\n", strlen("Invalid command format. Usage: CREATE ROOM channel_name room_name\n"), 0);
            }
        } 
        else if(strcmp(commands[0], "EDIT") == 0 && strcmp(commands[1], "CHANNEL") == 0){
            if(commands[2] != NULL && commands[3] != NULL){
                edit_channel(client_socket, client->logged_in_user, commands[2], commands[3]);
            } else {
                send(client_socket, "Invalid command format. Usage: EDIT CHANNEL old_channel_name new_channel_name\n", strlen("Invalid command format. Usage: EDIT CHANNEL old_channel_name new_channel_name\n"), 0);
            }
        }
        else if (strcmp(commands[0], "EDIT") == 0 && strcmp(commands[1], "ROOM") == 0) {
            if (commands[3] != NULL && commands[3] != NULL && commands[4] != NULL) {
                edit_room(client_socket, client->logged_in_user, client->logged_in_channel, commands[2], commands[4]);
            } else {
                send(client_socket, "Invalid command format. Usage: EDIT ROOM old_room_name TO new_room_name\n", strlen("Invalid command format. Usage: EDIT ROOM old_room_name TO new_room_name\n"), 0);
            }
        } else if (strcmp(commands[0], "DELETE") == 0 && strcmp(commands[1], "ROOM") == 0) {
            if (commands[3] != NULL) {
                del_room(client_socket, client->logged_in_user, client->logged_in_channel, commands[3]);
            } else {
                send(client_socket, "Invalid command format. Usage: DELETE ROOM room_name\n", strlen("Invalid command format. Usage: DELETE ROOM room_name\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "LIST") == 0 && strcmp(commands[1], "CHANNEL") == 0) {
            list_channels(client_socket, client->logged_in_user);
        } 
        else if(strcmp(commands[0], "LIST") == 0 && strcmp(commands[1], "ROOM") == 0){
            list_rooms(client_socket, client->logged_in_user, client->logged_in_channel);
        }
        else if (strcmp(commands[0], "EXIT") == 0) {
            exit_discorit(client_socket, client->logged_in_user, client->logged_in_channel, client->logged_in_room);
        } 
        else if (strcmp(commands[0], "DELETE") == 0 && strcmp(commands[1], "CHANNEL") == 0) {
            if (commands[2] != NULL) {
                del_channel(client_socket, client->logged_in_user, commands[2]);
            } else {
                send(client_socket, "Invalid command format. Usage: DELETE CHANNEL channel_name\n", strlen("Invalid command format. Usage: DELETE CHANNEL channel_name\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "BAN") == 0) {
            if (commands[1] != NULL) {
                ban_user(client_socket, client->logged_in_user, client->logged_in_channel, commands[1]);
            } else {
                send(client_socket, "Invalid command format. Usage: BAN username\n", strlen("Invalid command format. Usage: BAN username\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "UNBAN") == 0) {
            if (commands[1] != NULL) {
                unban_user(client_socket, client->logged_in_user, client->logged_in_channel, commands[1]);
            } else {
                send(client_socket, "Invalid command format. Usage: UNBAN username\n", strlen("Invalid command format. Usage: UNBAN username\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "REMOVE") == 0) {
            if (commands[1] != NULL) {
                remove_user(client_socket, client->logged_in_user, client->logged_in_channel, commands[1]);
            } else {
                send(client_socket, "Invalid command format. Usage: REMOVE username\n", strlen("Invalid command format. Usage: REMOVE username\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "EDIT") == 0 && strcmp(commands[1], "PROFILE") == 0) {
            if (commands[2] != NULL && strcmp(commands[3], "-p") == 0 && commands[4] != NULL) {
                edit_profile(client_socket, client->logged_in_user, commands[2], commands[4]);
            } else {
                send(client_socket, "Invalid command format. Usage: EDIT PROFILE username -p new_password\n", strlen("Invalid command format. Usage: EDIT PROFILE username -p new_password\n"), 0);
            }
        }
        else if (strcmp(commands[0], "JOIN") == 0 && is_in_channel == 0) {
            if (commands[1] != NULL && channel_exists(commands[1])) {
                char key[50];
                send(client_socket, "Key: ", strlen("Key: "), 0);
                int key_size = recv(client_socket, key, sizeof(key), 0);
                //check if the key is correct
                if (check_channel_key(commands[1], key)) {
                    join_channel(client_socket, client->logged_in_user, commands[1], key);
                } else {
                    send(client_socket, "Key salah tod\n", strlen("Key salah tod\n"), 0);
                }
            } else {
                send(client_socket, "Channel tidak ada \n", strlen("Channel tidak ada\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "JOIN") == 0 && client->logged_in_channel != NULL) {
            join_room(client_socket, client->logged_in_user, commands[1]);
        } 
        else if (strcmp(commands[0], "LIST") == 0 && strcmp(commands[1], "USERS") == 0) {
            if (commands[2] == NULL) {
                list_users(client_socket, client->logged_in_user);
            } else {
                send(client_socket, "Invalid command format. Usage: LIST USERS\n", strlen("Invalid command format. Usage: LIST USERS\n"), 0);
            }
        } 
        else if (strcmp(commands[0], "CHAT") == 0) {
            if (client->logged_in_channel != NULL && client->logged_in_room != NULL && commands[1] != NULL) {
                chat(client_socket, client->logged_in_user, client->logged_in_channel, client->logged_in_room, commands[1]);
            } else {
                send(client_socket, "Invalid command format. Usage: CHAT \"text\"\n", strlen("Invalid command format. Usage: CHAT \"text\"\n"), 0);
            }
        } else if (strcmp(commands[0], "SEE") == 0 && strcmp(commands[1], "CHAT") == 0) {
            if (client->logged_in_channel != NULL && client->logged_in_room != NULL) {
                see_chat(client_socket, client->logged_in_user, client->logged_in_channel, client->logged_in_room);
            } else {
                send(client_socket, "Invalid command format. Usage: SEE CHAT\n", strlen("Invalid command format. Usage: SEE CHAT\n"), 0);
            }
        } else if (strcmp(commands[0], "EDIT") == 0 && strcmp(commands[1], "CHAT") == 0) {
            if (client->logged_in_channel != NULL && client->logged_in_room != NULL && commands[2] != NULL && commands[3] != NULL) {
                edit_chat(client_socket, client->logged_in_user, client->logged_in_channel, client->logged_in_room, atoi(commands[2]), commands[3]);
            } else {
                send(client_socket, "Invalid command format. Usage: EDIT CHAT id \"text\"\n", strlen("Invalid command format. Usage: EDIT CHAT id \"text\"\n"), 0);
            }
        } else if (strcmp(commands[0], "DEL") == 0 && strcmp(commands[1], "CHAT") == 0) {
            if (client->logged_in_channel != NULL && client->logged_in_room != NULL && commands[2] != NULL) {
                del_chat(client_socket, client->logged_in_user, client->logged_in_channel, client->logged_in_room, atoi(commands[2]));
            } else {
                send(client_socket, "Invalid command format. Usage: DEL CHAT id\n", strlen("Invalid command format. Usage: DEL CHAT id\n"), 0);
            }
        }
        else {
            send(client_socket, "Unknown command\n", strlen("Unknown command\n"), 0);
        }

        memset(buffer, 0, sizeof(buffer));
    }

    // Client disconnected
    if (read_size == 0) {
        // Client disconnected
    } else if (read_size == -1) {
        perror("Receive failed");
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] == client) {
            free(clients[i]);
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
    pthread_exit(NULL);
}

int client_id(int client_fd, char *username){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL &&
            strstr(clients[i]->logged_in_user, username) != NULL){
            return i;
        }
    }
}

void register_user(int client_fd, char *username, char *password) {
    FILE *file = fopen(USERS_FILE, "a+");
    if (!file) {
        perror("Failed to open USERS_FILE");
        send(client_fd, "Server error. Please try again later.\n", strlen("Server error. Please try again later.\n"), 0);
        return;
    }

    
    char line[256];
    bool username_exists = false;
    while (fgets(line, sizeof(line), file)) {
        
        char *existing_username = strtok(line, ",");
        
        while(existing_username != NULL){

            if(strcmp(existing_username, username) == 0){
                username_exists = true;
                break;
            }
            existing_username = strtok(NULL, ","); 
        }
    }

    if (username_exists) {
        send(client_fd, "username sudah terdaftar\n", strlen("username sudah terdaftar\n"), 0);
    } else {
        
        char hashed_password[128];
        strcpy(hashed_password, crypt(password, SALT));

        
        if(user_count == 0){
            fprintf(file, "%d,%s,%s,%s\n", user_count+1, username, hashed_password, "root");
        } else {
            fprintf(file, "%d,%s,%s,%s\n", user_count+1, username, hashed_password, "user");
        }
        fclose(file);

        send(client_fd, "username berhasil register\n", strlen("username berhasil register\n"), 0);

        
        log_activity(username, "REGISTER", "User registered");

        
        FILE *auth_file = fopen(AUTH_FILE, "a+");
        if (!auth_file) {
            perror("Failed to open AUTH_FILE");
            return;
        }
        fprintf(auth_file, "%s,user\n", username);
        fclose(auth_file);
    }
}

// Function to handle user login
void login(int client_fd, char *username, char *password) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        perror("Failed to open USERS_FILE");
        send(client_fd, "Server error. Please try again later.\n", strlen("Server error. Please try again later.\n"), 0);
        return;
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
        send(client_fd, "Username tidak terdaftar\n", strlen("Username tidak terdaftar\n"), 0);
    } else {
        char input_hashed_password[128];
        strcpy(input_hashed_password, crypt(password, stored_password));

        if (strcmp(stored_password, input_hashed_password) != 0) {
            send(client_fd, "Password salah\n", strlen("Password salah\n"), 0);
        } else {
            send(client_fd, "username berhasil login\n", strlen("username berhasil login\n"), 0);

            log_activity(username, "LOGIN", "User logged in");

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i] != NULL && clients[i]->socket == client_fd) {
                    strcpy(clients[i]->logged_in_user, username);
                    strcpy(clients[i]->logged_in_role, "user");
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        }
    }
}


void log_activity(char *username, char *action, char *description) {
    FILE *log_file = fopen(USERS_LOG_FILE, "a");
    if (!log_file) {
        perror("Failed to open USERS_LOG_FILE");
        return;
    }

    time_t now;
    time(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", localtime(&now));

    fprintf(log_file, "[%s][%s] %s: %s\n", timestamp, username, action, description);
    fclose(log_file);
}

bool channel_exists(const char *channel_name) {
    FILE *file = fopen(CHANNELS_FILE, "r");
    if (!file) {
        perror("Failed to open CHANNELS_FILE");
        return false;
    }

    char line[256];
    bool found = false;
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        token = strtok(NULL, ","); // Get the channel name
        if (token != NULL && strstr(token, channel_name) != NULL) {
            
            found = true;
            break;
        }
    }

    fclose(file);
    return found;
}

void list_channels(int client_fd, char *username) {
    FILE *file = fopen(CHANNELS_FILE, "r");
    if (!file) {
        perror("Failed to open CHANNELS_FILE");
        send(client_fd, "Server error. Please try again later.\n", strlen("Server error. Please try again later.\n"), 0);
        return;
    }

    char response[BUFFER_SIZE] = "[";
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");  // Skip the first token (channel ID)
        token = strtok(NULL, ",");  // Get the second token (channel name)
        if (token != NULL) {
            strcat(response, token);
            strcat(response, " ");
        }
    }
    fclose(file);

    // Remove the trailing space if any
    if (response[strlen(response) - 1] == ' ') {
        response[strlen(response) - 1] = '\0';
    }
    strcat(response, "]\n");

    send(client_fd, response, strlen(response), 0);
}

void join_channel(int client_fd, char *username, char *channel_name, char *key) {
    // Check if the user is already in the channel
    bool user_already_in_channel = false;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL &&
            strcmp(clients[i]->logged_in_user, username) == 0 &&
            strcmp(clients[i]->logged_in_channel, channel_name) == 0) {
            user_already_in_channel = true;
            send(client_fd, "Anda sudah bergabung di channel ini\n", strlen("Anda sudah bergabung di channel ini\n"), 0);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (user_already_in_channel) {
        return; // Exit function if user is already in the channel
    }

    // Add user to the channel
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && clients[i]->socket == client_fd) {
            strcpy(clients[i]->logged_in_channel, channel_name);
            log_activity(username, "JOIN CHANNEL", channel_name);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // Append user to auth.csv

    char path[256];
    sprintf(path, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/admin/auth.csv", clients[client_id(client_fd, username)]->logged_in_channel);
    FILE *auth_check = fopen(path, "r");
    if (!auth_check) {
        perror("Failed to open AUTH_FILE");
        return;
    }
    // Check if the user is already in the channel
    bool alr_exists = false;
    char line[256];
    while (fgets(line, sizeof(line), auth_check)) {
        char *token = strtok(line, ",");
        char *name = strtok(NULL, ",");
        if (name != NULL && strcmp(name, username) == 0) {
            alr_exists = true;
            break;
        }
    }

    fclose(auth_check);
    if(!alr_exists){
    
        FILE *auth_file = fopen(path, "a");
        if (!auth_file) {
            perror("Failed to open AUTH_FILE");
            return;
        }
        
        fprintf(auth_file, "%d,%s,user\n", client_id(client_fd, username), username);
        fclose(auth_file);
    }

    // Send confirmation message to client
    char message[BUFFER_SIZE];
    sprintf(message, "Channel %s berhasil dimasuki\n", channel_name);
    is_in_channel = 1;
    send(client_fd, message, strlen(message), 0);
}

bool check_channel_key(const char *channel_name, const char *key) {
    FILE *file = fopen(CHANNELS_FILE, "r");
    if (!file) {
        perror("Failed to open CHANNELS_FILE");
        return false;
    }

    char line[256];
    char channel_key[50];
    bool channel_found = false;
    bool key_matched = true;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (token != NULL) {
            token = strtok(NULL, ",");
            if (token != NULL && strstr(token, channel_name) == 0) {
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strcpy(channel_key, token);
                    channel_key[strcspn(channel_key, "\n")] = '\0';  // Remove newline character
                    channel_found = true;
                    break;
                }
            }
        }
    }

    fclose(file);

    if (channel_found) {
        if (strstr(key, channel_key) == 0) {
            key_matched = false;
        }
    }

    return key_matched;
}

void list_rooms(int client_fd, char *username, char *channel_name) {
    FILE *file = fopen(CHANNELS_FILE, "r");
    if (!file) {
        perror("Failed to open CHANNELS_FILE");
        send(client_fd, "Server error. Please try again later.\n", strlen("Server error. Please try again later.\n"), 0);
        return;
    }

    char line[256];
    bool channel_found = false;
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (token != NULL && strcmp(token, channel_name) == 0) {
            channel_found = true;
            break;
        }
    }

    fclose(file);

    if (!channel_found) {
        send(client_fd, "Channel tidak ditemukan\n", strlen("Channel tidak ditemukan\n"), 0);
        return;
    }

    char rooms_file[256];
    sprintf(rooms_file, "%s_rooms.csv", channel_name);

    file = fopen(rooms_file, "r");
    if (!file) {
        return;
    }

    char response[BUFFER_SIZE] = "[";
    while (fgets(line, sizeof(line), file)) {
        char *room_name = strtok(line, ",");
        if (room_name != NULL) {
            strcat(response, room_name);
            strcat(response, " ");
        }
    }
    fclose(file);

    strcat(response, "]\n");

    send(client_fd, response, strlen(response), 0);
}

void join_room(int client_fd, char *username, char *room_name) {

    pthread_mutex_lock(&clients_mutex);
    pthread_mutex_unlock(&clients_mutex);

    // Add user to the channel
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && clients[i]->socket == client_fd) {
            log_activity(username, "JOIN ROOM", room_name);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // Append user to auth.csv
    int room_exists = 0;
    char path[256];
    sprintf(path, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/", clients[client_id(client_fd, username)]->logged_in_channel);
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, room_name) == 0) {
                room_exists = 1;
                break;
            }
        }
        closedir(d);
    }
    // Send confirmation message to client
    if(room_exists == 0) {
        send(client_fd, "Room tidak ada\n", strlen("Room tidak ada\n"), 0);
        return;
    }
    else{
        char message[BUFFER_SIZE];
        sprintf(message, "Room %s berhasil dimasuki\n", room_name);
        send(client_fd, message, strlen(message), 0);
    }
}


void list_users(int client_fd, char *username) {
    // Construct the path to auth.csv based on the client's logged-in channel
    char users_file[256];
    sprintf(users_file, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/admin/auth.csv", clients[client_id(client_fd, username)]->logged_in_channel);

    // Open the auth.csv file
    FILE *file = fopen(users_file, "r");
    if (!file) {
        // If file opening fails, send an error message to the client
        send(client_fd, "Belum ada user di channel ini\n", strlen("Belum ada user di channel ini\n"), 0);
        return;
    }

    char response[BUFFER_SIZE] = "["; // Start the response string with '['
    char line[256];

    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        // Use strtok to tokenize the line based on ','
        char *token = strtok(line, ",");
        
        // The second token (index 1) is the username, assuming auth.csv structure is consistent
        token = strtok(NULL, ","); // Move to the next token (username)

        if (token != NULL) {
            // Append the username to the response string
            strcat(response, token);
            strcat(response, " ");
        }
    }
    fclose(file);

    // Append closing bracket and newline to the response
    strcat(response, "]\n");

    // Send the response to the client
    send(client_fd, response, strlen(response), 0);
}

void chat(int client_fd, char *username, char *channel_name, char *room_name, char *message) {
    char chat_file_path[256];
    sprintf(chat_file_path, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s/chat.csv", channel_name, room_name);

    // printf("Channel: %s, Room: %s\n", channel_name, room_name);

    // Create the directory if it does not exist
    char dir_path[256];
    sprintf(dir_path, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s/", channel_name, room_name);
    mkdir(dir_path, 0777);

    FILE *file = fopen(chat_file_path, "a+");
    if (!file) {
        perror("Failed to open chat.csv");
        send(client_fd, "Server error. Please try again later.\n", strlen("Server error. Please try again later.\n"), 0);
        return;
    }

    // Get current time
    time_t rawtime;
    struct tm *info;
    char timestamp[80];
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", info);

    // Determine the next message ID by reading existing messages
    int message_id = 0;
    char last_line[256];
    fseek(file, 0, SEEK_SET);
    while (fgets(last_line, sizeof(last_line), file)) {
        int current_id;
        sscanf(last_line, "%*s\t%d\t%*s\t%*s", &current_id);
        if (current_id > message_id) {
            message_id = current_id;
        }
    }
    message_id++;

    // Write to the file with the correct format
    fprintf(file, "%s\t%d\t%s\t%s\n", timestamp, message_id, username, message);
    fclose(file);

    // Send the response to the client
    char response[BUFFER_SIZE];
    sprintf(response, "[%s] \"%s\" successfully sent\n", timestamp, message);
    send(client_fd, response, strlen(response), 0);
}

void see_chat(int client_fd, char *username, char *channel_name, char *room_name) {
    char chat_file_path[256];
    sprintf(chat_file_path, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s/chat.csv", channel_name, room_name);

    FILE *file = fopen(chat_file_path, "r");
    if (!file) {
        return;
    }

    char response[BUFFER_SIZE] = "";
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        strcat(response, line);
    }
    fclose(file);

    send(client_fd, response, strlen(response), 0);
}

void edit_chat(int client_fd, char *username, char *channel_name, char *room_name, int message_id, char *new_message) {
    pthread_mutex_lock(&clients_mutex);
    bool user_found = false;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL &&
            strcmp(clients[i]->logged_in_user, username) == 0 &&
            strcmp(clients[i]->logged_in_channel, channel_name) == 0 &&
            strcmp(clients[i]->logged_in_room, room_name) == 0) {
            user_found = true;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (!user_found) {
        return;
    }

    char chat_file[256];
    sprintf(chat_file, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/chat.csv", channel_name);

    FILE *file = fopen(chat_file, "r+");
    if (!file) {
        send(client_fd, "Gagal membuka chat untuk diedit\n", strlen("Gagal membuka chat untuk diedit\n"), 0);
        return;
    }

    char line[256];
    char edited_message[256];
    bool message_found = false;
    while (fgets(line, sizeof(line), file)) {
        int id;
        char *token = strtok(line, " ");
        sscanf(token, "[%*[^]]][%d]", &id);
        if (id == message_id) {
            char timestamp[80];
            sscanf(line, "[%[^]]][%*d]", timestamp);
            sprintf(edited_message, "[%s][%d][%s] \"%s\"\n", timestamp, message_id, username, new_message);

            fseek(file, -strlen(line), SEEK_CUR);
            fprintf(file, "%s", edited_message);
            message_found = true;
            break;
        }
    }

    fclose(file);

    if (!message_found) {
        send(client_fd, "Pesan dengan ID tersebut tidak ditemukan\n", strlen("Pesan dengan ID tersebut tidak ditemukan\n"), 0);
        return;
    }

    send(client_fd, "Pesan berhasil diedit\n", strlen("Pesan berhasil diedit\n"), 0);
}

void del_chat(int client_fd, char *username, char *channel_name, char *room_name, int message_id) {
    pthread_mutex_lock(&clients_mutex);
    bool user_found = false;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL &&
            strcmp(clients[i]->logged_in_user, username) == 0 &&
            strcmp(clients[i]->logged_in_channel, channel_name) == 0 &&
            strcmp(clients[i]->logged_in_room, room_name) == 0) {
            user_found = true;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (!user_found) {
        return;
    }

    char chat_file[256];
    sprintf(chat_file, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s/chat.csv", channel_name, room_name);

    // Open the chat file for reading and writing
    FILE *file = fopen(chat_file, "r+");
    if (!file) {
        send(client_fd, "Gagal membuka chat untuk dihapus\n", strlen("Gagal membuka chat untuk dihapus\n"), 0);
        return;
    }

    // Find and delete the specified message_id
    char line[256];
    char edited_message[256];
    bool message_found = false;
    while (fgets(line, sizeof(line), file)) {
        int id;
        sscanf(line, "[%*[^]]][%d]", &id);
        if (id == message_id) {
            message_found = true;
            break;
        }
    }

    if (!message_found) {
        fclose(file);
        send(client_fd, "Pesan dengan ID tersebut tidak ditemukan\n", strlen("Pesan dengan ID tersebut tidak ditemukan\n"), 0);
        return;
    }

    // Create a temporary file to copy all lines except the one to delete
    FILE *temp_file = fopen("temp_chat.csv", "w");
    if (!temp_file) {
        fclose(file);
        send(client_fd, "Gagal membuat file sementara\n", strlen("Gagal membuat file sementara\n"), 0);
        return;
    }

    // Copy all lines except the one to delete
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        int id;
        sscanf(line, "[%*[^]]][%d]", &id);
        if (id != message_id) {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Replace the original file with the temporary file
    if (rename("temp_chat.csv", chat_file) != 0) {
        send(client_fd, "Gagal menghapus pesan\n", strlen("Gagal menghapus pesan\n"), 0);
        return;
    }

    send(client_fd, "Pesan berhasil dihapus\n", strlen("Pesan berhasil dihapus\n"), 0);
}

void create_channel(int client_fd, char *username, char *channel_name, char *key) {
    // Validate if the user is logged in and is an admin or root
    pthread_mutex_lock(&clients_mutex);
    client_info *user_info = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            user_info = clients[i];
            break;
        }
    }

    if (user_info == NULL) {
        pthread_mutex_unlock(&clients_mutex);
        send(client_fd, "Anda tidak terdaftar atau sudah logout\n", strlen("Anda tidak terdaftar atau sudah logout\n"), 0);
        return;
    }
    pthread_mutex_unlock(&clients_mutex);

    FILE *file = fopen(CHANNELS_FILE, "a+");
    if (!file) {
        send(client_fd, "Gagal membuka file channels\n", strlen("Gagal membuka file channels\n"), 0);
        return;
    }

    // Check if the channel name already exists
    char line[256];
    bool channel_exists = false;
    int dummy_id;
    char existing_channel[50];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,%49[^,],", &dummy_id, existing_channel);
        if (strcmp(existing_channel, channel_name) == 0) {
            channel_exists = true;
            break;
        }
    }
    fclose(file);

    if (channel_exists) {
        send(client_fd, "Channel sudah ada\n", strlen("Channel sudah ada\n"), 0);
        return;
    }

    // Write new channel to file
    file = fopen(CHANNELS_FILE, "a");
    if (!file) {
        send(client_fd, "Gagal membuka file channels\n", strlen("Gagal membuka file channels\n"), 0);
        return;
    }
    static int channel_count = 0;  // Make sure to initialize channel_count properly based on existing channels
    fprintf(file, "%d,%s,%s\n", ++channel_count, channel_name, key);
    fclose(file);

    // Create directory structure for the new channel
    char channel_path[256];
    sprintf(channel_path, "DiscorIT/%s", channel_name);
    create_directory(channel_path);

    char admin_path[256];
    sprintf(admin_path, "DiscorIT/%s/admin", channel_name);
    create_directory(admin_path);

    char auth_file[256];
    sprintf(auth_file, "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/admin/auth.csv", channel_name);
    create_file(auth_file);

    // Add the user as an admin in the new channel's auth.csv
    file = fopen(auth_file, "a");
    if (!file) {
        send(client_fd, "Gagal membuka file auth\n", strlen("Gagal membuka file auth\n"), 0);
        return;
    }
    fprintf(file, "%d,%s,admin\n", user_info->socket, username);  
    fclose(file);

    char user_log_file[256];
    sprintf(user_log_file, "DiscorIT/%s/admin/user.log", channel_name);
    create_file(user_log_file);

    // Log the creation in users.log
    char log_file[256] = USERS_LOG_FILE;
    FILE *log = fopen(log_file, "a");
    if (!log) {
        send(client_fd, "Gagal membuka file log\n", strlen("Gagal membuka file log\n"), 0);
        return;
    }

    time_t now;
    time(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", localtime(&now));

    fprintf(log, "[%s] Channel %s dibuat oleh %s\n", timestamp, channel_name, username);
    fclose(log);

    send(client_fd, "Channel berhasil dibuat\n", strlen("Channel berhasil dibuat\n"), 0);
}

void create_directory(const char *path) {
    if (mkdir(path, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Failed to create directory");
            exit(1);
        }
    }
}

void create_file(const char *path) {
    FILE *file = fopen(path, "a");
    if (!file) {
        perror("Failed to create file");
        exit(1);
    }
    fclose(file);
}

void edit_channel(int client_fd, char *username, char *old_channel_name, char *new_channel_name) {
    // Validate if the user is logged in and is an admin or root
    pthread_mutex_lock(&clients_mutex);
    client_info *user_info = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL &&
            strcmp(clients[i]->logged_in_user, username) == 0) {
            user_info = clients[i];
            break;
        }
    }

    if (user_info == NULL) {
        pthread_mutex_unlock(&clients_mutex);
        send(client_fd, "Anda tidak terdaftar atau sudah logout\n", strlen("Anda tidak terdaftar atau sudah logout\n"), 0);
        return;
    }

    // Check if the user has admin or root role in any channel
    bool is_admin_or_root = false;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL &&
            (strcmp(clients[i]->logged_in_role, "admin") == 0 ||
             strcmp(clients[i]->logged_in_role, "root") == 0)) {
            is_admin_or_root = true;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    char channels_file[256] = CHANNELS_FILE;
    create_directory("DiscorIT");
    FILE *file = fopen(channels_file, "r+");
    if (!file) {
        send(client_fd, "Gagal membuka file channels\n", strlen("Gagal membuka file channels\n"), 0);
        return;
    }

    char line[256];
    bool channel_found = false;
    while (fgets(line, sizeof(line), file)) {
            char *token = strtok(line, ",");
            char *channel_name = strtok(NULL, ",");
            if (channel_name != NULL && strcmp(channel_name, old_channel_name) == 0) {
                channel_found = true;
                break;
        }
    }

    if (!channel_found) {
        send(client_fd, "Channel tidak ditemukan\n", strlen("Channel tidak ditemukan\n"), 0);
        return;
    }
    DIR *d;
    struct dirent *dir;
    char path[256];
    snprintf(path, sizeof(path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/");
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, old_channel_name) == 0) {
                char new_channel_path[256];
                char old_channel_path[256];

                snprintf(old_channel_path, sizeof(old_channel_path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s", old_channel_name);
                snprintf(new_channel_path, sizeof(new_channel_path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s", new_channel_name);
                pid_t child_id;
                int status;
                child_id = fork();
                if (child_id == 0) {
                    char *argv[] = {"mv", old_channel_path, new_channel_path, NULL};
                    execv("/bin/mv", argv);
                }
                else {
                    while ((wait(&status)) > 0);
                }
                break;
            }
        }
        closedir(d);
    }

    // Open a temporary file to copy all lines except the one to edit
    FILE *temp_file = fopen("temp_channels.csv", "w");
    if (!temp_file) {
        fclose(file);
        send(client_fd, "Gagal membuat file sementara\n", strlen("Gagal membuat file sementara\n"), 0);
        return;
    }

    // Copy all lines except the one to edit
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        char *channel_name = strtok(NULL, ",");
        if (channel_name != NULL && strcmp(channel_name, old_channel_name) != 0) {
            fprintf(temp_file, "%s", new_channel_name);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Replace the original file with the temporary file
    if (rename("temp_channels.csv", channels_file) != 0) {
        send(client_fd, "Gagal mengedit channel\n", strlen("Gagal mengedit channel\n"), 0);
        return;
    }

    // Append the new channel name to channels.csv
    file = fopen(channels_file, "a");
    if (!file) {
        send(client_fd, "Gagal menambahkan channel baru\n", strlen("Gagal menambahkan channel baru\n"), 0);
        return;
    }

    fprintf(file, "%d,%s,%s\n", 1, new_channel_name, "nokey");
    fclose(file);

    // Log the edit in users.log
    char log_file[256] = "users.log";
    FILE *log = fopen(log_file, "a");
    if (!log) {
        send(client_fd, "Gagal membuka file log\n", strlen("Gagal membuka file log\n"), 0);
        return;
    }

    time_t now = time(NULL);
    char *time_str = asctime(localtime(&now));
    time_str[strlen(time_str) - 1] = '\0'; // Remove newline character

    fprintf(log, "[%s] Channel %s diubah oleh %s menjadi %s\n", time_str, old_channel_name, username, new_channel_name);
    fclose(log);

    send(client_fd, "Channel berhasil diubah\n", strlen("Channel berhasil diubah\n"), 0);
    

}

void del_channel(int client_fd, char *username, char *channel_name) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        // Check if the user is the owner or has permission to delete the channel
        if (strcmp(current_client->logged_in_channel, channel_name) == 0) {
            
            // Remove the channel
            FILE *file = fopen(CHANNELS_FILE, "r");
            FILE *temp = fopen("temp.csv", "w");
            char line[256];

            while (fgets(line, sizeof(line), file)) {
                char *stored_channel = strtok(line, ",");
                if (stored_channel && strcmp(stored_channel, channel_name) != 0) {
                    fprintf(temp, "%s", line);
                }
            }

            fclose(file);
            fclose(temp);
            remove(CHANNELS_FILE);
            rename("temp.csv", CHANNELS_FILE);

            // Notify and remove users in the channel
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i] != NULL) {
                    strcpy(clients[i]->logged_in_channel, "");
                    strcpy(clients[i]->logged_in_room, "");
                    send(clients[i]->socket, "The channel you were in has been deleted\n", strlen("The channel you were in has been deleted\n"), 0);
                }
            }

            printf("Channel %s deleted\n", channel_name);
            send(client_fd, "Channel successfully deleted\n", strlen("Channel successfully deleted\n"), 0);

            // Log the activity
            log_activity(username, "DELETE CHANNEL", channel_name);
            char channel_path[256] = "/home/purofuro/Fico/fpsisop/final/DiscorIT/";
            strcat(channel_path, channel_name);
            remove_function(channel_path);

        } else {
            printf("You are not authorized to delete this channel\n");
            send(client_fd, "You are not authorized to delete this channel\n", strlen("You are not authorized to delete this channel\n"), 0);
        }
    } else {
        printf("User not found\n");
        send(client_fd, "User not found\n", strlen("User not found\n"), 0);
    }

    pthread_mutex_unlock(&clients_mutex);
}

void create_room(int client_fd, char *username, char *channel_name, char *room_name) {
    // Validate if the user is logged in and is part of the channel
    pthread_mutex_lock(&clients_mutex);
    client_info *user_info = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            user_info = clients[i];
            break;
        }
    }

    if (user_info == NULL) {
        pthread_mutex_unlock(&clients_mutex);
        send(client_fd, "Anda tidak terdaftar atau sudah logout\n", strlen("Anda tidak terdaftar atau sudah logout\n"), 0);
        return;
    }

    // Check if the user is part of the channel
    char auth_file[256];
    sprintf(auth_file, "DiscorIT/%s/admin/auth.csv", channel_name);

    FILE *file = fopen(auth_file, "r");
    if (!file) {
        send(client_fd, "Gagal membuka file auth\n", strlen("Gagal membuka file auth\n"), 0);
        return;
    }

    char line[256];
    bool is_authorized = false;
    while (fgets(line, sizeof(line), file)) {
        char stored_username[50];
        sscanf(line, "%*d,%49[^,],", stored_username);
        if (strstr(stored_username, username) != 0) {
            is_authorized = true;
            break;
        }
    }
    fclose(file);

    char room_path[256];
    sprintf(room_path, "DiscorIT/%s/%s", channel_name, room_name);
    create_directory(room_path);

    char chat_file[256];
    sprintf(chat_file, "DiscorIT/%s/%s/chat.csv", channel_name, room_name);
    create_file(chat_file);

    log_activity(username, "CREATE ROOM", room_name);

    send(client_fd, "Room berhasil dibuat\n", strlen("Room berhasil dibuat\n"), 0);
}

void edit_room(int client_fd, char *username, char *channel_name, char *old_room_name, char *new_room_name) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    DIR *d;
    struct dirent *dir;
    char path[256];
    int found = 0;
    snprintf(path, sizeof(path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/", channel_name);
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, old_room_name) == 0) {
                char new_room_path[256];
                char old_room_path[256];

                snprintf(new_room_path, sizeof(new_room_path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s", channel_name, new_room_name);
                snprintf(old_room_path, sizeof(old_room_path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s", channel_name, old_room_name);
                pid_t child_id;
                int status;
                child_id = fork();
                if (child_id == 0) {
                    char *argv[] = {"mv", old_room_path, new_room_path, NULL};
                    execv("/bin/mv", argv);
                }
                else {
                    while ((wait(&status)) > 0);
                }
                found = 1;
                break;
            }
        }
        closedir(d);
    }
    if(found == 0) {
        send(client_fd, "Room tidak ditemukan\n", strlen("Room tidak ditemukan\n"), 0);
        return;
    }
    else{
        send(client_fd, "Room berhasil diubah\n", strlen("Room berhasil diubah\n"), 0);
    }
    log_activity(username, "EDIT ROOM", new_room_name);
    pthread_mutex_unlock(&clients_mutex);
}

void del_room(int client_fd, char *username, char *channel_name, char *room_name) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        if (strcmp(current_client->logged_in_channel, channel_name) == 0) {
            
            remove_function(room_name);
            struct dirent *dir;
            char path[256];
            snprintf(path, sizeof(path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/", channel_name);
            DIR *d = opendir(path);
            if (d) {
                while ((dir = readdir(d)) != NULL) {
                    if(strcmp(dir->d_name, room_name) == 0) {
                        char room_path[256];
                        snprintf(room_path, sizeof(room_path), "/home/purofuro/Fico/fpsisop/final/DiscorIT/%s/%s", channel_name, room_name);
                        remove_function(room_path);
                        send(client_fd, "Room berhasil dihapus\n", strlen("Room berhasil dihapus\n"), 0);
                        break;
                    }
                }
                closedir(d);
            }

        } else {
            printf("Room %s not found in channel %s\n", room_name, channel_name);
            send(client_fd, "Room not found\n", strlen("Room not found\n"), 0);
        }

    } else {
            printf("You are not in the specified channel\n");
            send(client_fd, "You are not in the specified channel\n", strlen("You are not in the specified channel\n"), 0);
        }

    pthread_mutex_unlock(&clients_mutex);
}

void del_all_rooms(int client_fd, char *username, char *channel_name) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        if (strcmp(current_client->logged_in_channel, channel_name) == 0) {
            printf("All rooms deleted from %s channel\n", channel_name);
        } else {
            printf("You are not authorized to delete all rooms in this channel\n");
        }
    } else {
        printf("User not found\n");
    }

    pthread_mutex_unlock(&clients_mutex);
}

void ban_user(int client_fd, char *username, char *channel_name, char *user_to_ban) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        if (strcmp(current_client->logged_in_channel, channel_name) == 0) {
            printf("%s banned from %s channel\n", user_to_ban, channel_name);
        } else {
            printf("You are not authorized to ban users from this channel\n");
        }
    } else {
        printf("User not found\n");
    }

    pthread_mutex_unlock(&clients_mutex);
}

void unban_user(int client_fd, char *username, char *channel_name, char *user_to_unban) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        if (strcmp(current_client->logged_in_channel, channel_name) == 0) {
            printf("%s unbanned from %s channel\n", user_to_unban, channel_name);
        } else {
            printf("You are not authorized to unban users from this channel\n");
        }
    } else {
        printf("User not found\n");
    }

    pthread_mutex_unlock(&clients_mutex);
}

void remove_user(int client_fd, char *username, char *channel_name, char *user_to_remove) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        if (strcmp(current_client->logged_in_channel, channel_name) == 0) {
            printf("%s removed from %s channel\n", user_to_remove, channel_name);
        } else {
            printf("You are not authorized to remove users from this channel\n");
        }
    } else {
        printf("User not found\n");
    }

    pthread_mutex_unlock(&clients_mutex);
}

void edit_profile(int client_fd, char *username, char *new_username, char *new_password) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        // Update username and/or password
        strncpy(current_client->logged_in_user, new_username, sizeof(current_client->logged_in_user) - 1);

        printf("Profile edited successfully\n");
    } else {
        printf("User not found\n");
    }

    pthread_mutex_unlock(&clients_mutex);
}

void exit_discorit(int client_fd, char *username, char *channel_name, char *room_name) {
    pthread_mutex_lock(&clients_mutex);

    client_info *current_client = NULL;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL && strcmp(clients[i]->logged_in_user, username) == 0) {
            current_client = clients[i];
            break;
        }
    }

    if (current_client != NULL) {
        if (strcmp(current_client->logged_in_channel, channel_name) == 0 && strcmp(current_client->logged_in_room, room_name) == 0) {
            
            strcpy(current_client->logged_in_channel, "");
            strcpy(current_client->logged_in_room, "");
            
            // Log the activity

            // Send a message to the client

            // Close the client's socket
            close(current_client->socket);

            // Remove the client from the clients array
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i] == current_client) {
                    free(clients[i]);
                    clients[i] = NULL;
                    break;
                }
            }
        } else {
        }
    } else {
        send(client_fd, "User not found\n", strlen("User not found\n"), 0);
    }

    pthread_mutex_unlock(&clients_mutex);
}

void remove_function(char* path) {

    pid_t child_id;
    int status;
    child_id = fork();
    if (child_id == 0) {
        char *argv[] = {"remove", "-r", "-f", path,  NULL};
        execv("/bin/rm", argv);
    }
    else {
        while ((wait(&status)) > 0);
    }
}
