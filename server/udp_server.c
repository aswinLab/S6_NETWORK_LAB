#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 5000
#define MAXLINE 1000


int main(){
    struct sockaddr_in server, client;
    char buffer[MAXLINE];
    bzero(&server, sizeof(server));
    int sockfd, ln;


    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0){
        perror("Socket failed");
        exit(0);
    }

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htonl(PORT);
    server.sin_family = AF_INET;

    socklen_t ln_server = sizeof(server);
    socklen_t ln_client = sizeof(client);

    if(bind(sockfd, (struct sockaddr *) &server, sizeof server) < 0){
        perror("Bind failed");
        exit(0);
    }

    printf("Server is listening\n");

    for(;;){
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &ln_server);
        buffer[n] = '\0';

        if(strcmp(buffer, "end")){
            printf("Connection closed");
        }
        puts(buffer);

        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, ln_client);

    }
}