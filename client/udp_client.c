#include <stdio.h>
#include <unistd.h>
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
        int sockfd, cli_len;
        char buffer[MAXLINE];

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        cli_len = sizeof(client);

        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htonl(PORT);
        server.sin_family = AF_INET;

        if(sockfd < 0){
            perror("Socket failed");
            exit(0);
        }
        
        while(connect(sockfd, (struct sockaddr *) &server, sizeof(server))<0){
            printf("Attempting connection again in...10 seconds\n");
            sleep(10);
        }

        printf("Connection is established with server\n");

        for(;;){
            printf("Client :");
            fgets(buffer, sizeof(buffer), stdin);
            sendto(sockfd, buffer, sizeof(buffer), 0,(struct sockaddr *) &server, sizeof(server));

            bzero(buffer, sizeof(buffer));

            int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &server);

            buffer[n] = '\0';

            printf("Server : ");
            puts(buffer);

        }
}