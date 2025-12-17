// Client side code for a tcp connection

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
    struct sockaddr_in client,server;
    int client_conn, client_size, sockfd;
    
    char buff_cli[50] = "", buff_serv[50] = "";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("Socket failed");
        exit(0);
    }

    server.sin_family = AF_INET;
    server.sin_port = 8000;
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    while(connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("Connection failed, trying again in 10 sec...");
        sleep(10);
    }

    for(;;){
        printf("\nClient : \t");
        scanf("%s", buff_cli);
        if(strcmp(buff_cli, "end") == 0) break;
        send(sockfd, buff_cli, sizeof(buff_cli), 0);

        recv(sockfd, buff_serv, sizeof(buff_serv), 0);
        if(strcmp(buff_serv, "end") == 0) break;
        printf("\nServer : \t%s", buff_serv);
    }

    close(sockfd);
}