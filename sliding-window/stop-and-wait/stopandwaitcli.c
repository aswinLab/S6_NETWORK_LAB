#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FRAME_SIZE 2

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    char message[1024];
    char frame[FRAME_SIZE];
    int ack;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter message: ");
    scanf("%s",message);

    int len = strlen(message);

    printf("\nStarting frame transmission\n\n");

    for(int i=0;i<len;i+=2)
    {
        frame[0] = message[i];
        frame[1] = (i+1 < len) ? message[i+1] : '_';

        printf("Sending frame: %c%c\n",frame[0],frame[1]);

        sendto(sockfd,frame,FRAME_SIZE,0,
               (struct sockaddr*)&server_addr,addr_len);

        recvfrom(sockfd,&ack,sizeof(int),0,
                 (struct sockaddr*)&server_addr,&addr_len);

        printf("ACK received\n\n");

        sleep(1);
    }

    char end[2] = "-9";

    sendto(sockfd,end,2,0,
           (struct sockaddr*)&server_addr,addr_len);

    printf("Transmission finished\n");

    close(sockfd);

    return 0;
}