#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
  
int main(int argc, char *argv[]) {
    struct hostent *address;
    int sock, n, port;
    struct sockaddr_in serv_addr;
    char baper[1024];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    port = atoi(argv[2]);
    memset(&serv_addr, '0', sizeof(serv_addr));
    if(sock < 0){
        perror("error opening socket");
        address = gethostbyname(argv[1]);
        if(address == NULL) fprintf(stderr, "no such host");
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    else printf("Connected!\n");

    char untukprint[100][100] = {"(a) Gap", "(b) Fuel", "(c) Tire", "(d) Tire Change"};
    sleep(1);
    while(1){

        printf("Puro says choose:\n");
        for(int i = 0; i < 4; i++){
            printf("%s\n", untukprint[i]);
        }
        memset(baper, 0, sizeof(baper));
        fgets(baper, sizeof(baper), stdin);
        baper[strcspn(baper, "\n")] = '\0';
        send(sock, baper, strlen(baper), 0);
        memset(baper, 0, sizeof(baper));
        recv(sock, baper, sizeof(baper), 0);
        printf("[PakDok Said]: [%s]\n", baper);
    }
    close(sock);
    return 0;
}