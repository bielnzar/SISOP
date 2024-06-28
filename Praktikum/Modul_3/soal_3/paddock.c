#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
//Aksi dari action anjay
#include "actions.c"

void doraemon(){
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    umask(0);

    chdir("/");

    printf("PID: %d\n", getpid());
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

}

int main(int argc, char *argv[]) {
    doraemon();
    if(argc < 2){
        fprintf(stderr, "No port provided!\n");
        exit(1);
    }
    
    int server_fd, new_socket, n, port;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char baper[1024];
      
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("Waiting for incoming connections...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    
        while(1){
            bzero(baper, 1024);
            char *sendback;
            n = read(new_socket, baper, 1024);
            char FirstWord[100], SecondWord[100], ThirdWord[100];
            sscanf(baper, "%s %s %s", FirstWord, SecondWord, ThirdWord);
            if(strcmp(FirstWord, "Gap") == 0 && isdigit(SecondWord[0])){
                Printlog("Driver", FirstWord, SecondWord);
                Printlog("Paddock", FirstWord, GapFunc(baper));
                sendback = GapFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
            }
            else if(strcmp(FirstWord, "Fuel") == 0 && isdigit(SecondWord[0])){
                Printlog("Drive]", FirstWord, SecondWord);
                Printlog("Paddock", FirstWord, FuelFunc(baper));
                sendback = FuelFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
                
            }
            else if(strcmp(FirstWord, "Tire") == 0 && isdigit(SecondWord[0])){
                Printlog("Driver", FirstWord, SecondWord);
                Printlog("Paddock", FirstWord, TireFunc(baper));
                sendback = TireFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
            
            } 
            else if(strcmp(FirstWord, "Tire") == 0 && strcmp(SecondWord, "Change") == 0 && (strcmp(ThirdWord, "Soft") == 0 || strcmp(ThirdWord, "Medium") == 0)){
                char allword[1000];
                sprintf(allword, "%s %s", SecondWord, ThirdWord);
                Printlog("Driver", FirstWord, allword);
                Printlog("Paddock", FirstWord, ChangeFunc(baper));
                sendback = ChangeFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
            }
            else{
                sendback = NoneOfYourInputIsCorrectYouKnow();
                Printlog("Driver", "Wrong", "Input");
                send(new_socket, sendback, strlen(sendback), 0);
            }
        
        }
        close(new_socket);
        
    }
    close(server_fd);

    return 0;
}
