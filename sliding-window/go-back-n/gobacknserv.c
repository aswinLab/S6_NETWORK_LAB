#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FRAME_SIZE 2
#define MAX_MSG 1024

typedef struct {
    int seq;
    char data[FRAME_SIZE];
} frame;

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    frame f;
    int ack;

    int expected = 0;

    char message[MAX_MSG];
    int index = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));

    printf("GBN Server started\n\n");

    while(1)
    {
        recvfrom(sockfd,&f,sizeof(f),0,
                 (struct sockaddr*)&client_addr,&addr_len);

        if(f.seq == -99)
            break;

        printf("Frame %d received [%c%c]\n",f.seq,f.data[0],f.data[1]);

        if(f.seq == expected)
        {
            message[index++] = f.data[0];
            message[index++] = f.data[1];

            ack = f.seq;
            expected++;

            printf("Frame accepted\n");
        }
        else
        {
            ack = expected - 1;
            printf("Frame discarded (out of order)\n");
        }

        sendto(sockfd,&ack,sizeof(int),0,
               (struct sockaddr*)&client_addr,addr_len);

        printf("ACK %d sent\n\n",ack);
    }

    message[index] = '\0';

    printf("Reconstructed message: %s\n",message);

    close(sockfd);

    return 0;
}