#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 9090
#define BUF 1024
#define STORE "storage"

void send_msg(int fd, const char *msg) {
    send(fd, msg, strlen(msg), 0);
}

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

/* Strip directory components — only keep the basename */
const char *safe_name(const char *name) {
    const char *p = strrchr(name, '/');
    return p ? p + 1 : name;
}

void do_list(int client) {
    DIR *dir = opendir(STORE);
    if (!dir) { send_msg(client, "ERR\n"); return; }

    char out[4096] = "";
    struct dirent *e;
    while ((e = readdir(dir)))
        if (e->d_name[0] != '.')
            { strcat(out, e->d_name); strcat(out, "\n"); }
    closedir(dir);

    char hdr[64];
    sprintf(hdr, "LIST %ld\n", (long)strlen(out));
    send_msg(client, hdr);
    send(client, out, strlen(out), 0);
}

void do_get(int client, const char *name) {
    char path[512];
    name = safe_name(name);
    sprintf(path, "%s/%s", STORE, name);

    struct stat st;
    if (stat(path, &st) < 0) { send_msg(client, "ERR\n"); return; }

    char hdr[64];
    sprintf(hdr, "SIZE %ld\n", (long)st.st_size);
    send_msg(client, hdr);

    int fd = open(path, O_RDONLY);
    if (fd < 0) { send_msg(client, "ERR\n"); return; }

    char buf[BUF];
    int n;
    while ((n = read(fd, buf, BUF)) > 0)
        send(client, buf, n, 0);
    close(fd);
}

void do_put(int client, const char *name, long size) {
    char path[512];
    name = safe_name(name);
    sprintf(path, "%s/%s", STORE, name);

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { send_msg(client, "ERR\n"); return; }

    send_msg(client, "READY\n");

    char buf[BUF];
    long rem = size;
    while (rem > 0) {
        int chunk = rem > BUF ? BUF : rem;
        int n = recv(client, buf, chunk, 0);
        if (n <= 0) break;
        write(fd, buf, n);
        rem -= n;
    }
    close(fd);
    send_msg(client, "OK\n");
}

void do_delete(int client, const char *name) {
    char path[512];
    name = safe_name(name);
    sprintf(path, "%s/%s", STORE, name);
    send_msg(client, remove(path) == 0 ? "Deleted\n" : "Delete failed\n");
}

void handle_client(int client) {
    char line[BUF];
    while (recv_line(client, line, BUF) > 0) {
        printf("Command: %s\n", line);

        char cmd[64] = "", arg[256] = "";
        long fsize = 0;
        sscanf(line, "%s %s %ld", cmd, arg, &fsize);

        if      (!strcmp(cmd, "PING"))   send_msg(client, "PONG\n");
        else if (!strcmp(cmd, "HELP"))   send_msg(client,
            "HELP\nPING\nLIST\nGET <file>\nPUT <file>\nDELETE <file>\nQUIT\nEND\n");
        else if (!strcmp(cmd, "LIST"))   do_list(client);
        else if (!strcmp(cmd, "GET"))    do_get(client, arg);
        else if (!strcmp(cmd, "PUT"))    do_put(client, arg, fsize);
        else if (!strcmp(cmd, "DELETE")) do_delete(client, arg);
        else if (!strcmp(cmd, "QUIT"))   break;
        else                            send_msg(client, "Unknown command\n");
    }
    close(client);
    printf("Client disconnected\n");
}

int main() {
    mkdir(STORE, 0755);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        { perror("bind"); return 1; }
    if (listen(sfd, 5) < 0)
        { perror("listen"); return 1; }

    printf("Server running on port %d\n", PORT);

    /* Accept clients in a loop */
    while (1) {
        int client = accept(sfd, NULL, NULL);
        if (client < 0) { perror("accept"); continue; }
        printf("Client connected\n");
        handle_client(client);
    }

    close(sfd);
    return 0;
}