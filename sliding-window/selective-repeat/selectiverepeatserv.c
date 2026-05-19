#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FRAME_SIZE 2
#define WINDOW_SIZE 4
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

    int base = 0;

    frame buffer[WINDOW_SIZE];
    int received[WINDOW_SIZE] = {0};

    char message[MAX_MSG];
    int index = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));

    printf("Selective Repeat Server started\n\n");

    while(1)
    {
        recvfrom(sockfd,&f,sizeof(f),0,
                 (struct sockaddr*)&client_addr,&addr_len);

        if(f.seq == -99)
            break;

        printf("Frame %d received [%c%c]\n",f.seq,f.data[0],f.data[1]);

        if(f.seq >= base && f.seq < base + WINDOW_SIZE)
        {
            int index_buf = f.seq % WINDOW_SIZE;

            buffer[index_buf] = f;
            received[index_buf] = 1;

            ack = f.seq;

            printf("Frame buffered\n");
        }
        else
        {
            ack = f.seq;
            printf("Frame outside window\n");
        }

        sendto(sockfd,&ack,sizeof(int),0,
               (struct sockaddr*)&client_addr,addr_len);

        printf("ACK %d sent\n\n",ack);

        while(received[base % WINDOW_SIZE])
        {
            frame temp = buffer[base % WINDOW_SIZE];

            message[index++] = temp.data[0];
            message[index++] = temp.data[1];

            received[base % WINDOW_SIZE] = 0;

            base++;
        }
    }

    message[index] = '\0';

    printf("Reconstructed message: %s\n",message);

    close(sockfd);

    return 0;
}