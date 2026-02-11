#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/select.h>

#define PORT 9090
#define TIMEOUT 2

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
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    Packet pkt;
    Ack ack;

    char message[1024];

    uint8_t seq = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);

    int i = 0;

    while (message[i] != '\0') {

        pkt.seq = seq;
        pkt.pad = 0;

        uint16_t payload = 0;
        payload |= ((uint8_t)message[i]) << 8;

        if (message[i+1] != '\0')
            payload |= (uint8_t)message[i+1];

        pkt.data = htons(payload);

        while (1) {

            printf("Sending seq %d\n", seq);

            sendto(sockfd, &pkt, sizeof(Packet), 0,
                   (struct sockaddr *)&server, len);

            fd_set fds;
            struct timeval tv;

            FD_ZERO(&fds);
            FD_SET(sockfd, &fds);

            tv.tv_sec = TIMEOUT;
            tv.tv_usec = 0;

            int rv = select(sockfd + 1, &fds, NULL, NULL, &tv);

            if (rv == 0) {
                printf("Timeout, resending...\n");
                continue;
            }

            recvfrom(sockfd, &ack, sizeof(Ack), 0,
                     (struct sockaddr *)&server, &len);

            if (ack.ack == seq) {
                printf("ACK %d received\n", ack.ack);
                seq ^= 1;
                break;
            }
        }

        i += 2;
    }

    close(sockfd);
    return 0;
}
