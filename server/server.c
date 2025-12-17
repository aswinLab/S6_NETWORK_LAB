// Server side code for tcp connection in c.

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
    struct sockaddr_in client, server;
    int client_conn, client_size, sock_fd;

    char buff_one[10] = "",buff_two[10] = "";

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_fd == -1){
        perror("Socket failed");
        exit(0);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = 8000;
    if(bind(sock_fd, (struct sockaddr *) &server, sizeof server) < 0){
        perror("Bind Failed");
        exit(0);
    }

    if(listen(sock_fd, 5) < 0) perror("Listen failed");
    printf("Server is listening\n");

    client_size = sizeof(client);

    client_conn = accept(sock_fd, (struct sockaddr *) &client, &client_size);


    for(;;){
        if(recv(client_conn, buff_one, sizeof(buff_one), 0) < 0){
            perror("recv failed");
            exit(0);
        }

        // if(strcmp(buff_one, "end") == 0){
        //     break;
        // }

        printf("\nClient : \t%s", buff_one);
        printf("\nServer : \t");
        fgets(buff_two, sizeof(buff_two), stdin);
        send(client_conn, buff_two, sizeof(buff_two), 0);
        if(strcmp(buff_two, "end") == 0){
            break;
        }
    }

    close(client_conn);
    close(sock_fd);


}