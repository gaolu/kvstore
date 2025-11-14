// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_LINE 512

static void die(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) die("socket");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) die("connect");

    printf("Connected. Type commands like:\n");
    printf("  SET key value\n  GET key\n  DEL key\n  QUIT\n");

    char sendbuf[MAX_LINE];
    char recvbuf[MAX_LINE];

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(sendbuf, sizeof(sendbuf), stdin)) break;

        size_t len = strlen(sendbuf);
        if (len == 0) continue;
        if (sendbuf[len - 1] != '\n') {
            sendbuf[len] = '\n';
            sendbuf[len + 1] = '\0';
        }

        if (send(sock, sendbuf, strlen(sendbuf), 0) <= 0) break;

        ssize_t n = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (n <= 0) break;
        recvbuf[n] = '\0';
        printf("< %s", recvbuf);

        if (strncmp(sendbuf, "QUIT", 4) == 0) break;
    }

    close(sock);
    return 0;
}
