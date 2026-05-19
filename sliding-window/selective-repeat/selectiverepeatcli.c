#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FRAME_SIZE 2
#define WINDOW_SIZE 4

typedef struct {
    int seq;
    char data[FRAME_SIZE];
} frame;

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    char message[1024];

    frame frames[512];

    int acked[512] = {0};

    int ack;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter message: ");
    scanf("%s",message);

    int len = strlen(message);

    int total_frames = (len + 1) / 2;

    for(int i=0;i<total_frames;i++)
    {
        frames[i].seq = i;

        frames[i].data[0] = message[i*2];
        frames[i].data[1] = (i*2+1 < len) ? message[i*2+1] : '_';
    }

    int base = 0;
    int next_seq = 0;

    printf("\nStarting Selective Repeat transmission\n\n");

    while(base < total_frames)
    {
        while(next_seq < base + WINDOW_SIZE && next_seq < total_frames)
        {
            printf("Sending Frame %d [%c%c]\n",
                   frames[next_seq].seq,
                   frames[next_seq].data[0],
                   frames[next_seq].data[1]);

            sendto(sockfd,&frames[next_seq],sizeof(frame),0,
                   (struct sockaddr*)&server_addr,addr_len);

            next_seq++;
        }

        recvfrom(sockfd,&ack,sizeof(int),0,
                 (struct sockaddr*)&server_addr,&addr_len);

        printf("ACK %d received\n",ack);

        acked[ack] = 1;

        while(acked[base])
            base++;

        sleep(1);
        printf("\n");
    }

    frame end;
    end.seq = -99;

    sendto(sockfd,&end,sizeof(end),0,
           (struct sockaddr*)&server_addr,addr_len);

    printf("Transmission complete\n");

    close(sockfd);

    return 0;
}