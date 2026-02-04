#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 9090
#define BUF_SIZE 1024


int main() {
    int sockfd;
    char buffer[BUF_SIZE];


    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);


    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }


    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");


    printf("Enter message: ");
    fgets(buffer, BUF_SIZE, stdin);


    sendto(sockfd, buffer, strlen(buffer), 0,
    (struct sockaddr *)&server, server_len);


    printf("Waiting for server reply...\n");


    int nbytes = recvfrom(sockfd, buffer, BUF_SIZE, 0,
    (struct sockaddr *)&server, &server_len);


    buffer[nbytes] = '\0';
    printf("Server replied: %s\n", buffer);


    close(sockfd);
    return 0;
}