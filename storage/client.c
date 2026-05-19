#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 9090
#define BUF 1024

int recv_line(int fd, char *buf, int size) {
    int i = 0;
    char c;
    while (i < size - 1) {
        int n = recv(fd, &c, 1, 0);
        if (n <= 0) return n;
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

/* Receive exactly 'len' bytes into buf */
int recv_all(int fd, char *buf, long len) {
    long got = 0;
    while (got < len) {
        int n = recv(fd, buf + got, len - got, 0);
        if (n <= 0) return -1;
        got += n;
    }
    return 0;
}

void do_put(int sock, const char *filename) {
    struct stat st;
    if (stat(filename, &st) < 0) {
        printf("File not found: %s\n", filename);
        return;
    }

    char buf[BUF];
    sprintf(buf, "PUT %s %ld\n", filename, (long)st.st_size);
    send(sock, buf, strlen(buf), 0);

    if (recv_line(sock, buf, BUF) <= 0) return;
    if (strcmp(buf, "READY") != 0) { printf("Server: %s\n", buf); return; }

    int fd = open(filename, O_RDONLY);
    if (fd < 0) { printf("Cannot open file\n"); return; }

    int n;
    while ((n = read(fd, buf, BUF)) > 0)
        send(sock, buf, n, 0);
    close(fd);

    if (recv_line(sock, buf, BUF) > 0)
        printf("%s\n", buf);
}

void do_get(int sock, const char *filename) {
    char buf[BUF];
    sprintf(buf, "GET %s\n", filename);
    send(sock, buf, strlen(buf), 0);

    if (recv_line(sock, buf, BUF) <= 0) return;

    long fsize;
    if (sscanf(buf, "SIZE %ld", &fsize) != 1) {
        printf("File not found on server\n");
        return;
    }

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { printf("Cannot create local file\n"); return; }

    long rem = fsize;
    while (rem > 0) {
        int chunk = rem > BUF ? BUF : rem;
        int n = recv(sock, buf, chunk, 0);
        if (n <= 0) break;
        write(fd, buf, n);
        rem -= n;
    }
    close(fd);
    printf("Downloaded %s (%ld bytes)\n", filename, fsize);
}

void do_list(int sock) {
    char buf[BUF];
    send(sock, "LIST\n", 5, 0);

    if (recv_line(sock, buf, BUF) <= 0) return;

    long size;
    if (sscanf(buf, "LIST %ld", &size) != 1) {
        printf("Error: %s\n", buf);
        return;
    }

    if (size == 0) { printf("(empty)\n"); return; }

    char *data = malloc(size + 1);
    if (recv_all(sock, data, size) < 0) {
        printf("Transfer error\n");
        free(data);
        return;
    }
    data[size] = '\0';
    printf("%s", data);
    free(data);
}

void do_help(int sock) {
    char buf[BUF];
    send(sock, "HELP\n", 5, 0);

    /* Read lines until we get END marker */
    while (recv_line(sock, buf, BUF) > 0) {
        if (strcmp(buf, "END") == 0) break;
        printf("  %s\n", buf);
    }
}

void do_simple(int sock, const char *cmd) {
    char buf[BUF];
    sprintf(buf, "%s\n", cmd);
    send(sock, buf, strlen(buf), 0);

    if (recv_line(sock, buf, BUF) > 0)
        printf("%s\n", buf);
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in srv = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    };
    inet_pton(AF_INET, "127.0.0.1", &srv.sin_addr);

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0)
        { perror("connect"); return 1; }

    char input[BUF];

    while (1) {
        printf("ftp> ");
        if (!fgets(input, BUF, stdin)) break;
        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0') continue;

        char cmd[64] = "", arg[256] = "";
        sscanf(input, "%s %s", cmd, arg);

        if      (!strcmp(cmd, "PUT"))    do_put(sock, arg);
        else if (!strcmp(cmd, "GET"))    do_get(sock, arg);
        else if (!strcmp(cmd, "LIST"))   do_list(sock);
        else if (!strcmp(cmd, "HELP"))   do_help(sock);
        else if (!strcmp(cmd, "QUIT"))   { send(sock, "QUIT\n", 5, 0); break; }
        else                            do_simple(sock, input);
    }

    close(sock);
    return 0;
}