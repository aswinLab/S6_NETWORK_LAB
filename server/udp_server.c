#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../sliding-window/stop-and-wait/packet.h"
#include "../sliding-window/stop-and-wait/reciever.h"


#define PORT 9090
#define BUF_SIZE 1024


int main() {
int sockfd;
char buffer[BUF_SIZE];


struct sockaddr_in server, client;
socklen_t client_len = sizeof(client);


sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if (sockfd < 0) {
    perror("socket");
    exit(1);
}

receiver_init(sockfd);

memset(&server, 0, sizeof(server));
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons(PORT);


if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("bind");
    exit(1);
}


printf("UDP Server listening on port %d\n", PORT);


while (1) {
    int nbytes = recvfrom(sockfd, buffer, BUF_SIZE, 0,
    (struct sockaddr *)&client, &client_len);


    if (nbytes < 0) {
        perror("recvfrom");
        continue;
    }

    connect(sockfd, (struct sockaddr*)&client, client_len);


    Packet pkt;
    memcpy(&pkt, buffer, sizeof(Packet));
    receiver_on_packet(pkt);


    send(sockfd, buffer, nbytes, 0);
    }


    close(sockfd);
    return 0;
}