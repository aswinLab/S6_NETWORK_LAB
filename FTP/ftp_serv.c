#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>   // basename()

#define PORT 9090
#define BUFFER 4096
#define STORAGE_DIR "uploads"

/* ---------- Reliable send ---------- */
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

/* ---------- Read one CRLF terminated line ---------- */
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

/* ---------- Ensure uploads directory exists ---------- */
void ensure_storage_dir() {
    struct stat st = {0};
    if (stat(STORAGE_DIR, &st) == -1) {
        mkdir(STORAGE_DIR, 0755);
    }
}

/* ---------- Handle one client ---------- */
void handle_client(int client_fd) {

    char buffer[BUFFER];

    while (1) {

        if (recv_line(client_fd, buffer, BUFFER) <= 0)
            break;

        printf("Received: %s", buffer);

        /* ---------------- PUT ---------------- */
        if (strncmp(buffer, "PUT", 3) == 0) {

            char filename[256];
            size_t filesize;

            sscanf(buffer, "PUT %255s %zu", filename, &filesize);

            /* Strip path components */
            char *base = basename(filename);

            char safe_path[512];
            snprintf(safe_path, sizeof(safe_path),
                     "%s/%s", STORAGE_DIR, base);

            printf("Saving to: %s (%zu bytes)\n",
                   safe_path, filesize);

            send_all(client_fd, "READY\r\n", 7);

            int fd = open(safe_path,
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                send_all(client_fd, "ERR\r\n", 5);
                continue;
            }

            size_t remaining = filesize;

            while (remaining > 0) {
                ssize_t r = recv(client_fd, buffer,
                                 remaining > BUFFER ? BUFFER : remaining, 0);
                if (r <= 0) break;

                write(fd, buffer, r);
                remaining -= r;
            }

            close(fd);

            printf("Upload complete\n");
            send_all(client_fd, "OK\r\n", 4);
        }

        /* ---------------- GET ---------------- */
        else if (strncmp(buffer, "GET", 3) == 0) {

            char filename[256];
            sscanf(buffer, "GET %255s", filename);

            char *base = basename(filename);

            char safe_path[512];
            snprintf(safe_path, sizeof(safe_path),
                     "%s/%s", STORAGE_DIR, base);

            struct stat st;
            if (stat(safe_path, &st) < 0) {
                send_all(client_fd, "ERR\r\n", 5);
                continue;
            }

            char header[64];
            sprintf(header, "SIZE %ld\r\n", st.st_size);
            send_all(client_fd, header, strlen(header));

            if (recv_line(client_fd, buffer, BUFFER) <= 0)
                continue;

            int fd = open(safe_path, O_RDONLY);
            if (fd < 0)
                continue;

            ssize_t r;
            while ((r = read(fd, buffer, BUFFER)) > 0) {
                send_all(client_fd, buffer, r);
            }

            close(fd);

            printf("Download sent\n");
        }

        /* ---------------- LIST ---------------- */
        else if (strncmp(buffer, "LIST", 4) == 0) {

            DIR *dir = opendir(STORAGE_DIR);
            if (!dir) continue;

            char listing[8192] = {0};
            size_t len = 0;

            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {

                if (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0)
                    continue;

                size_t name_len = strlen(entry->d_name);

                if (len + name_len + 2 >= sizeof(listing))
                    break;

                strcpy(listing + len, entry->d_name);
                len += name_len;
                listing[len++] = '\n';
            }

            closedir(dir);

            char header[64];
            sprintf(header, "LISTING %ld\r\n", len);

            send_all(client_fd, header, strlen(header));
            send_all(client_fd, listing, len);

            printf("Listing sent\n");
        }

        /* ---------------- QUIT ---------------- */
        else if (strncmp(buffer, "QUIT", 4) == 0) {
            break;
        }

        else {
            send_all(client_fd, "UNKNOWN\r\n", 9);
        }
    }

    close(client_fd);
    printf("Client disconnected\n");
}

/* ---------- Main ---------- */
int main() {

    ensure_storage_dir();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET,
               SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd,
             (struct sockaddr*)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("FTP Server running on port %d\n", PORT);

    while (1) {

        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected\n");
        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}