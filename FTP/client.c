#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER 1024

int main()
{
    int sockfd;

    struct sockaddr_in server_addr;

    char command[BUFFER];
    char buffer[BUFFER];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));

    while(1)
    {
        printf("ftp> ");
        fgets(command,BUFFER,stdin);

        command[strcspn(command,"\n")] = 0;

        write(sockfd,command,strlen(command));

        if(strncmp(command,"LIST",4)==0)
        {
            while(1)
            {
                read(sockfd,buffer,BUFFER);

                if(strcmp(buffer,"END")==0)
                    break;

                printf("%s",buffer);
            }
        }

        else if(strncmp(command,"GET",3)==0)
        {
            char filename[100];
            sscanf(command,"GET %s",filename);

            FILE *fp = fopen(filename,"w");

            while(1)
            {
                read(sockfd,buffer,BUFFER);

                if(strcmp(buffer,"EOF")==0)
                    break;

                fputs(buffer,fp);
            }

            fclose(fp);

            printf("File downloaded\n");
        }

        else if(strncmp(command,"PUT",3)==0)
        {
            char filename[100];
            sscanf(command,"PUT %s",filename);

            FILE *fp = fopen(filename,"r");

            if(fp == NULL)
            {
                printf("File not found\n");
                continue;
            }

            while(fgets(buffer,BUFFER,fp))
                write(sockfd,buffer,strlen(buffer));

            strcpy(buffer,"EOF");
            write(sockfd,buffer,strlen(buffer));

            fclose(fp);
        }

        else if(strncmp(command,"QUIT",4)==0)
        {
            break;
        }
    }

    close(sockfd);

    return 0;

}