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
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
    int n;

    // 0) Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // 1) Bind IP and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(client_addr);

    printf("UDP server listening on port %d...\n", PORT);

    while (1) {

        // 2) Read message from client
        n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                     (struct sockaddr*)&client_addr, &addr_len);

        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';

        printf("Client says: %s\n", buffer);

        // 3) Send message back to client
        char reply[] = "Message received";

        sendto(sockfd, reply, strlen(reply), 0,
               (struct sockaddr*)&client_addr, addr_len);
    }

    // 4) Close socket (never reached unless server stops)
    close(sockfd);

    return 0;
}