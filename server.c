// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "kv_store.h"

#define PORT 9000
#define MAX_LINE 512

static void die(const char* msg) {
    perror(msg);
    exit(1);
}

// 简单读取一行（以 '\n' 结尾）
ssize_t recv_line(int fd, char* buf, size_t maxlen) {
    size_t n = 0;
    while (n < maxlen - 1) {
        char c;
        ssize_t r = recv(fd, &c, 1, 0);
        if (r == 1) {
            if (c == '\n') {
                buf[n] = '\0';
                return (ssize_t)n;
            }
            buf[n++] = c;
        } else if (r == 0) {
            // connection closed
            if (n == 0) return 0;
            buf[n] = '\0';
            return (ssize_t)n;
        } else {
            if (errno == EINTR) continue;
            return -1;
        }
    }
    buf[n] = '\0';
    return (ssize_t)n;
}

void handle_client(int client_fd) {
    char line[MAX_LINE];

    while (1) {
        ssize_t n = recv_line(client_fd, line, sizeof(line));
        if (n <= 0) {
            // client closed or error
            break;
        }

        // 解析命令：SET key value / GET key / DEL key / QUIT
        char cmd[8];
        char key[128];
        char value[256];

        memset(cmd, 0, sizeof(cmd));
        memset(key, 0, sizeof(key));
        memset(value, 0, sizeof(value));

        int num = sscanf(line, "%7s %127s %255[^\n]", cmd, key, value);

        if (num <= 0) {
            const char* resp = "ERR invalid command\n";
            send(client_fd, resp, strlen(resp), 0);
            continue;
        }

        if (strcmp(cmd, "QUIT") == 0) {
            const char* resp = "OK bye\n";
            send(client_fd, resp, strlen(resp), 0);
            break;
        } else if (strcmp(cmd, "SET") == 0) {
            if (num < 3) {
                const char* resp = "ERR usage: SET key value\n";
                send(client_fd, resp, strlen(resp), 0);
                continue;
            }
            int ret = kv_put(key, value);
            if (ret == 0) {
                const char* resp = "OK\n";
                send(client_fd, resp, strlen(resp), 0);
            } else {
                const char* resp = "ERR store full\n";
                send(client_fd, resp, strlen(resp), 0);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (num < 2) {
                const char* resp = "ERR usage: GET key\n";
                send(client_fd, resp, strlen(resp), 0);
                continue;
            }
            const char* v = kv_get(key);
            if (v) {
                char resp[512];
                snprintf(resp, sizeof(resp), "OK %s\n", v);
                send(client_fd, resp, strlen(resp), 0);
            } else {
                const char* resp = "ERR not found\n";
                send(client_fd, resp, strlen(resp), 0);
            }
        } else if (strcmp(cmd, "DEL") == 0) {
            if (num < 2) {
                const char* resp = "ERR usage: DEL key\n";
                send(client_fd, resp, strlen(resp), 0);
                continue;
            }
            int ret = kv_del(key);
            if (ret == 0) {
                const char* resp = "OK\n";
                send(client_fd, resp, strlen(resp), 0);
            } else {
                const char* resp = "ERR not found\n";
                send(client_fd, resp, strlen(resp), 0);
            }
        } else {
            const char* resp = "ERR unknown command\n";
            send(client_fd, resp, strlen(resp), 0);
        }
    }
}

// TODO: 第二节课 —— 在线程池中调用 handle_client
int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) die("socket");

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) die("bind");
    if (listen(listen_fd, 128) < 0) die("listen");

    printf("KV server listening on port %d...\n", PORT);

    kv_init();

    while (1) {
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);
        int client_fd = accept(listen_fd, (struct sockaddr*)&cliaddr, &clilen);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }
        // 单线程版本：直接在当前线程处理
        handle_client(client_fd);
        close(client_fd);
    }

    close(listen_fd);
    return 0;
}
