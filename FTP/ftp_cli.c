#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 9090
#define BUFFER 4096

ssize_t send_all(int fd, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;

    while (total < len) {
        ssize_t sent = send(fd, p + total, len - total, 0);
        if (sent <= 0) return -1;
        total += sent;
    }
    return total;
}

ssize_t recv_line(int fd, char *buffer, size_t max_len) {
    size_t total = 0;
    char c;

    while (total < max_len - 1) {
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0) return -1;

        buffer[total++] = c;

        if (total >= 2 &&
            buffer[total - 2] == '\r' &&
            buffer[total - 1] == '\n')
            break;
    }

    buffer[total] = '\0';
    return total;
}

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    char command[BUFFER];
    char buffer[BUFFER];

    while (1) {

        printf("ftp> ");
        fgets(command, BUFFER, stdin);

        /* PUT */
        if (strncmp(command, "put ", 4) == 0) {

            char filename[256];
            sscanf(command, "put %255s", filename);

            struct stat st;
            if (stat(filename, &st) < 0) {
                printf("File not found\n");
                continue;
            }

            sprintf(buffer, "PUT %s %ld\r\n", filename, st.st_size);
            send_all(sock, buffer, strlen(buffer));

            if (recv_line(sock, buffer, BUFFER) <= 0)
                continue;

            if (strncmp(buffer, "READY", 5) != 0)
                continue;

            int fd = open(filename, O_RDONLY);
            ssize_t r;
            while ((r = read(fd, buffer, BUFFER)) > 0)
                send_all(sock, buffer, r);
            close(fd);

            recv_line(sock, buffer, BUFFER);
            printf("%s", buffer);
        }

        /* GET */
        else if (strncmp(command, "get ", 4) == 0) {

            char filename[256];
            sscanf(command, "get %255s", filename);

            sprintf(buffer, "GET %s\r\n", filename);
            send_all(sock, buffer, strlen(buffer));

            if (recv_line(sock, buffer, BUFFER) <= 0)
                continue;

            long size;
            if (sscanf(buffer, "SIZE %ld", &size) != 1) {
                printf("Error retrieving file\n");
                continue;
            }

            send_all(sock, "READY\r\n", 7);

            int fd = open(filename,
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);

            long remaining = size;

            while (remaining > 0) {
                ssize_t r = recv(sock, buffer,
                                 remaining > BUFFER ? BUFFER : remaining, 0);
                write(fd, buffer, r);
                remaining -= r;
            }

            close(fd);
            printf("Download complete\n");
        }

        /* LIST */
        else if (strncmp(command, "list", 4) == 0) {

            send_all(sock, "LIST\r\n", 6);

            if (recv_line(sock, buffer, BUFFER) <= 0)
                continue;

            long size;
            sscanf(buffer, "LISTING %ld", &size);

            long remaining = size;

            while (remaining > 0) {
                ssize_t r = recv(sock, buffer,
                                 remaining > BUFFER ? BUFFER : remaining, 0);
                fwrite(buffer, 1, r, stdout);
                remaining -= r;
            }
        }

        /* QUIT */
        else if (strncmp(command, "quit", 4) == 0) {
            send_all(sock, "QUIT\r\n", 6);
            break;
        }
    }

    close(sock);
    return 0;
}