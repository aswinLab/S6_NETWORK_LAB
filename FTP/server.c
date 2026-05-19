#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);

    char buffer[BUFFER];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));

    listen(server_fd,5);

    printf("FTP Server running...\n");

    client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&addrlen);

    while(1)
    {
        memset(buffer,0,BUFFER);

        read(client_fd,buffer,BUFFER);

        if(strncmp(buffer,"LIST",4) == 0)
        {
            printf("Client requested file list\n");

            FILE *fp = popen("ls","r");

            while(fgets(buffer,BUFFER,fp))
                write(client_fd,buffer,strlen(buffer));

            pclose(fp);

            strcpy(buffer,"END");
            write(client_fd,buffer,strlen(buffer));
        }

        else if(strncmp(buffer,"GET",3) == 0)
        {
            char filename[100];
            sscanf(buffer,"GET %s",filename);

            FILE *fp = fopen(filename,"r");

            if(fp == NULL)
            {
                strcpy(buffer,"FILE NOT FOUND");
                write(client_fd,buffer,strlen(buffer));
                continue;
            }

            while(fgets(buffer,BUFFER,fp))
                write(client_fd,buffer,strlen(buffer));

            strcpy(buffer,"EOF");
            write(client_fd,buffer,strlen(buffer));

            fclose(fp);
        }

        else if(strncmp(buffer,"PUT",3) == 0)
        {
            char filename[100];
            sscanf(buffer,"PUT %s",filename);

            FILE *fp = fopen(filename,"w");

            while(1)
            {
                read(client_fd,buffer,BUFFER);

                if(strcmp(buffer,"EOF")==0)
                    break;

                fputs(buffer,fp);
            }

            fclose(fp);
        }

        else if(strncmp(buffer,"QUIT",4) == 0)
        {
            printf("Client disconnected\n");
            break;
        }
    }

    close(client_fd);
    close(server_fd);

    return 0;
}