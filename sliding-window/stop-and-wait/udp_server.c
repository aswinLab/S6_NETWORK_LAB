#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>

#define PORT 9090

typedef struct {
    uint8_t seq;
    uint8_t pad;
    uint16_t data;
} Packet;

typedef struct {
    uint8_t ack;
} Ack;

int main() {
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    Packet pkt;
    Ack ack;

    char buffer[1024];
    int index = 0;

    uint8_t expected = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("Server listening...\n");

    while (1) {
        recvfrom(sockfd, &pkt, sizeof(Packet), 0,
                 (struct sockaddr *)&client, &len);

        if (pkt.seq == expected) {

            uint16_t payload = ntohs(pkt.data);

            char c1 = (payload >> 8) & 0xFF;
            char c2 = payload & 0xFF;

            buffer[index++] = c1;
            if (c2 != '\0')
                buffer[index++] = c2;

            buffer[index] = '\0';

            printf("Received so far: %s\n", buffer);

            ack.ack = pkt.seq;
            expected ^= 1;
        }
        else {
            ack.ack = expected ^ 1;
        }

        sendto(sockfd, &ack, sizeof(Ack), 0,
               (struct sockaddr *)&client, len);
    }

    close(sockfd);
    return 0;
}
