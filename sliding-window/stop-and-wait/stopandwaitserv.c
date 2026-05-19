#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FRAME_SIZE 2
#define MAX_MSG 1024

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    char frame[FRAME_SIZE + 1];
    char message[MAX_MSG];
    int index = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));

    printf("Server started. Waiting for frames...\n\n");

    while(1)
    {
        recvfrom(sockfd,frame,FRAME_SIZE,0,
                 (struct sockaddr*)&client_addr,&addr_len);

        frame[FRAME_SIZE] = '\0';

        if(strcmp(frame,"-9")==0)  // first part of -99
        {
            printf("\nTermination frame received\n");
            break;
        }

        printf("Frame received: %s\n",frame);

        message[index++] = frame[0];
        message[index++] = frame[1];

        sleep(1);   // simulate delay

        int ack = 1;

        sendto(sockfd,&ack,sizeof(int),0,
               (struct sockaddr*)&client_addr,addr_len);

        printf("ACK sent\n\n");
    }

    message[index] = '\0';

    printf("\nConstructed message: %s\n",message);

    close(sockfd);

    return 0;
}