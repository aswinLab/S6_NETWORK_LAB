#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
    int n;

    // 1) Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    addr_len = sizeof(server_addr);

    while (1) {

        // 2) Read message from stdin
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr*)&server_addr, addr_len);

        // 3) Read message from server
        n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                     (struct sockaddr*)&server_addr, &addr_len);

        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';

        printf("Server reply: %s\n", buffer);
    }

    close(sockfd);

    return 0;
}